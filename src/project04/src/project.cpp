#include <complex>
#include <cstdlib>
#include <iostream>
#include <chrono>

#include <argparse/argparse.hpp>
#include <fmt/ostream.h>
#include <fmt/color.h>

#include "inputs.h"
#include "topology.h"
#include "parallel_fd.h"

constexpr int COLUMN_WIDTH = 80;
const static std::string eq_divider(COLUMN_WIDTH, '=');
const static std::string dash_divider(COLUMN_WIDTH, '-');

struct Header
{
    std::string title{ "NE 591 Project #04" };
    std::string author{ "Kirill Shumilov" };
    std::string date{ "03/21/2025" };
    std::string description{ "Solving Source Iteration Equations with Parallel Point-Jacobi" };


    [[nodiscard]] constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "{0:=^{1}}\n"
            "{2:s}\n"
            "Author: {3:s}\n"
            "Date: {4:s}\n"
            "{0:-^{1}}\n"
            "{5:s}\n"
            "{0:=^{1}}\n",
            "", COLUMN_WIDTH,
            title,
            author,
            date,
            description
        );
    }


    void echo(std::ostream& out) const { out << this->to_string(); }
};

template<std::floating_point T>
auto lup_solve(const RectangularRegion<T>& region)
{
    auto A = region.build_matrix();
    const auto b = region.source.data();

    const auto [P, result] = lup_factor_inplace(A);
    auto x = lup_solve(A, P, b);

    const auto inner_shape = region.grid.build_inner_indexer();

    return Matrix<T>{
        static_cast<std::size_t>(inner_shape.rows()), static_cast<std::size_t>(inner_shape.cols()),
        std::move(x),
    };
}


template<std::floating_point T>
constexpr auto get_local_stencil(const ConstantStencil2D<T>& global_stencil, const CartesianTopology& topology)
{
    const auto n_points_x = global_stencil.shape.inner_rows();
    const auto n_points_y = global_stencil.shape.inner_rows();
    const auto n_blocks_x = topology.n_blocks_x();
    const auto n_blocks_y = topology.n_blocks_y();

    if (n_points_x % n_blocks_x != 0 || n_points_y % n_blocks_y != 0)
    {
        throw std::invalid_argument("Invalid sqrt(P) must divide n");
    }

    const auto block_size_x = n_points_x / n_blocks_x;
    const auto block_size_y = n_points_y / n_blocks_y;

    return ConstantStencil2D<T>{
        {block_size_x + 2, block_size_y + 2},
        global_stencil.m_top,
        global_stencil.m_bottom,
        global_stencil.m_left,
        global_stencil.m_right,
        global_stencil.m_center
    };
}

template<std::floating_point T>
[[nodiscard]]
constexpr auto get_block_size_x(const ConstantStencil2D<T>& stencil)
{
   return stencil.shape.inner_rows();
}

template<std::floating_point T>
[[nodiscard]]
constexpr auto get_n_points_x(const ConstantStencil2D<T>& stencil, const CartesianTopology& topology) -> int
{
   return get_block_size_x(stencil) * topology.n_blocks_x();
}


template<std::floating_point T>
[[nodiscard]]
constexpr auto get_block_size_y(const ConstantStencil2D<T>& stencil)
{
   return stencil.shape.inner_cols();
}

template<std::floating_point T>
[[nodiscard]]
constexpr auto get_n_points_y(const ConstantStencil2D<T>& stencil, const CartesianTopology& topology) -> int
{
   return get_block_size_y(stencil) * topology.n_blocks_y();
}

template<std::floating_point T>
[[nodiscard]]
constexpr auto get_block_size(const ConstantStencil2D<T>& stencil) -> int
{
   return get_block_size_x(stencil) * get_block_size_y(stencil);
}


template<std::floating_point T>
auto read_input(int argc, char* argv[])
{
    argparse::ArgumentParser program{
        "shumilov_project03",
        "1.0",
        argparse::default_arguments::help,
    };
    program.add_argument("input").help("Path to input file.");

    try
    {
        program.parse_args(argc, argv);

        const auto filename = program.get<std::string>("input");
        return Inputs<T>::read_input_file(filename);
    }
    catch (const std::exception& err)
    {
        fmt::print(
            std::cerr,
            "\n{}: {}\n\n",
            fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Error: "
            ),
            err.what()
        );
        std::exit(EXIT_FAILURE);
    }
}


template<std::floating_point T>
[[nodiscard]]
auto create_block_datatypes(const int block_size_x, const int block_size_y, const int n_points_y)
{
    MPI_Datatype block{}, blocktype{};
    MPI_Type_vector(
        block_size_x,
        // Number of blocks
        block_size_y,
        // Number of elements per block
        n_points_y,
        get_mpi_type<T>(),
        &block
    );
    MPI_Type_commit(&block);

    MPI_Type_create_resized(block, 0, block_size_y * sizeof(T), &blocktype);
    MPI_Type_commit(&blocktype);

    return std::make_pair(block, blocktype);
}


[[nodiscard]]
auto create_block_displacements(const CartesianTopology& topology, const int block_size_x)
{
    std::vector<int> displacements{};
    for (const auto i : std::views::iota(0, topology.n_blocks_x()))
    {
        const auto block_offset = i * block_size_x * topology.n_blocks_y();
        for (const auto j : std::views::iota(0, topology.n_blocks_y()))
        {
            displacements.emplace_back(block_offset + j);
        }
    }
    return displacements;
}


[[nodiscard]]
auto create_source_count(const CartesianTopology& topology)
{
    const std::vector<int> count(topology.n_blocks(), 1);
    return count;
}


using real = long double;
constexpr int MANAGER_IDX = 0;

auto main(int argc, char* argv[]) -> int
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    auto iter_settings_dtype = create_mpi_iter_settings_type<real>();
    auto indexer2d_dtype = create_mpi_indexer2d_type();
    auto stencil_dtype = create_mpi_stencil_type<real>(indexer2d_dtype);

    const auto start = std::chrono::high_resolution_clock::now();

    try
    {
        const auto topology = CartesianTopology::from_mpi_comm(MPI_COMM_WORLD);

        FixedPointIterSettings<real> iter_settings{};
        ConstantStencil2D<real> global_stencil{};
        ConstantStencil2D<real> local_stencil{};
        std::vector<real> local_source_data{};

        if (topology.local_offset == MANAGER_IDX)
        {
            const Header header{};
            header.echo(std::cout);

            auto inputs = read_input<real>(argc, argv);

            inputs.echo(std::cout);

            if (inputs.algorithm != AxbAlgorithm::PointJacobi)
            {
                throw std::runtime_error("Only PJ algorithm (1) is allowed");
            }

            if (not inputs.region.grid.is_square())
            {
                throw std::invalid_argument("Must be a square grid");
            }

            iter_settings = inputs.iter_settings;
            MPI_Bcast(&iter_settings, 1, iter_settings_dtype, MANAGER_IDX, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            global_stencil = inputs.region.build_stencil();
            MPI_Bcast(&global_stencil, 1, stencil_dtype, MANAGER_IDX, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            local_stencil = get_local_stencil(global_stencil, topology);

            const int block_size_x = get_block_size_x(local_stencil);
            const int block_size_y = get_block_size_y(local_stencil);
            const int block_size = get_block_size(local_stencil);
            const int n_points_y = get_n_points_y(local_stencil, topology);

            const auto displ = create_block_displacements(topology, block_size_x);
            const auto source_count = create_source_count(topology);

            std::vector<real> data(block_size, 0);

            auto [block, blocktype] = create_block_datatypes<real>(block_size_x, block_size_y, n_points_y);
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Scatterv(
                inputs.region.source.data().data(), source_count.data(), displ.data(), blocktype,
                data.data(), data.size(), get_mpi_type<real>(),
                MANAGER_IDX, MPI_COMM_WORLD
            );

            std::swap(local_source_data, data);

            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Type_free(&block);
            MPI_Type_free(&blocktype);
        }
        else
        {
            MPI_Bcast(&iter_settings, 1, iter_settings_dtype, MANAGER_IDX, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Bcast(&global_stencil, 1, stencil_dtype, MANAGER_IDX, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            local_stencil = get_local_stencil(global_stencil, topology);

            const int block_size = get_block_size(local_stencil);
            std::vector<real> data(block_size, 0);

            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Scatterv(
                nullptr, nullptr, nullptr, MPI_DATATYPE_NULL,
                data.data(), data.size(), get_mpi_type<real>(),
                MANAGER_IDX, MPI_COMM_WORLD
            );

            std::swap(local_source_data, data);
            MPI_Barrier(MPI_COMM_WORLD);
        }

        const Matrix<real> local_source{
            static_cast<std::size_t>(local_stencil.shape.inner_rows()),
            static_cast<std::size_t>(local_stencil.shape.inner_cols()),
            std::move(local_source_data),
        };

        const ParallelFiniteDifference<real> fd {iter_settings, topology};
        const auto result = fd.solve(local_stencil, local_source);

        fmt::println("Rank {}, {}", topology.local_offset, result.to_string());

        if (topology.local_offset == MANAGER_IDX)
        {
            const int block_size_x = get_block_size_x(local_stencil);
            const int block_size_y = get_block_size_y(local_stencil);
            const int block_size = get_block_size(local_stencil);
            const int n_points_y = get_n_points_y(local_stencil, topology);

            const auto displ = create_block_displacements(topology, block_size_x);
            const auto source_count = create_source_count(topology);

            std::vector<real> data(block_size * topology.n_blocks(), 0);

            auto [block, blocktype] = create_block_datatypes<real>(block_size_x, block_size_y, n_points_y);
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Gatherv(
                result.u.data().data(), result.u.size(), get_mpi_type<real>(),
                data.data(), source_count.data(), displ.data(), blocktype,
                MANAGER_IDX, MPI_COMM_WORLD
            );

            FiniteDifferenceResult<real> global_result{
                Matrix<real>{
                    static_cast<std::size_t>(global_stencil.shape.inner_rows()),
                    static_cast<std::size_t>(global_stencil.shape.inner_cols()),
                    std::move(data),
                },
                result.converged
            };

            MPI_Reduce(
                &result.iters,
                &global_result.iters,
                1,
                MPI_INT,
                MPI_MAX,
                MANAGER_IDX,
                MPI_COMM_WORLD
            );

            MPI_Reduce(
                &result.iter_error,
                &global_result.iter_error,
                1,
                get_mpi_type<real>(),
                MPI_MAX,
                MANAGER_IDX,
                MPI_COMM_WORLD
            );

            MPI_Reduce(
                &result.max_abs_residual,
                &global_result.max_abs_residual,
                1,
                get_mpi_type<real>(),
                MPI_MAX,
                MANAGER_IDX,
                MPI_COMM_WORLD
            );

            const auto end = std::chrono::high_resolution_clock::now();

            fmt::print(
                std::cout,
                "................................................................................\n"
                "Rank {}: {}\n"
                "Execution time: {:%S} seconds.\n"
                "================================================================================\n",
                MANAGER_IDX,
                global_result.to_string(),
                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            );

            if (global_result.u.rows() <= 8)
                global_result.echo(std::cout);
            else
            {
                std::ofstream output{ "Flux" };
                if (!output.is_open())
                {
                    throw std::runtime_error(
                        fmt::format("Could not open: '{}'", "Flux")
                    ); // Indicate an error occurred
                }

                global_result.echo(output);
            }
        }
        else
        {
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Gatherv(
                result.u.data().data(), result.u.size(), get_mpi_type<real>(),
                nullptr, nullptr, nullptr, MPI_DATATYPE_NULL,
                MANAGER_IDX, MPI_COMM_WORLD
            );

            MPI_Reduce(
                &result.iters,
                nullptr,
                1,
                MPI_INT,
                MPI_MAX,
                MANAGER_IDX,
                MPI_COMM_WORLD
            );

            MPI_Reduce(
                &result.iter_error,
                nullptr,
                1,
                get_mpi_type<real>(),
                MPI_MAX,
                MANAGER_IDX,
                MPI_COMM_WORLD
            );

            MPI_Reduce(
                &result.max_abs_residual,
                nullptr,
                1,
                get_mpi_type<real>(),
                MPI_MAX,
                MANAGER_IDX,
                MPI_COMM_WORLD
            );
        }
    }
    catch (const std::exception& err)
    {
        if (get_local_rank(MPI_COMM_WORLD) == MANAGER_IDX)
        {
            fmt::print(
                std::cerr,
                "\n{}: {}\n\n",
                fmt::format(
                    fmt::emphasis::bold | fg(fmt::color::red),
                    "Error: "
                ),
                err.what()
            );
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        std::exit(EXIT_FAILURE);
    }

    MPI_Type_free(&iter_settings_dtype);
    MPI_Type_free(&indexer2d_dtype);
    MPI_Type_free(&stencil_dtype);

    MPI_Finalize();
}