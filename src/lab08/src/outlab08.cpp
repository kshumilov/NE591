#include <cstdlib>
#include <filesystem>

#include <argparse/argparse.hpp>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "io.h"
#include "outlab08.h"

using real = long double;
constexpr int MANAGER_IDX = 0;



auto create_mpi_grid_type() -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    constexpr MPI_Datatype types[n_fields] = {MPI_INT, MPI_INT};

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Grid, num_angular_points),
        offsetof(Grid, num_spatial_cells)
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}

template<std::floating_point T>
auto create_mpi_iter_settings_type() -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    MPI_Datatype types[n_fields] = {
        get_mpi_type<T>(),
        MPI_INT,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(FixedPointIterSettings<T>, tolerance),
        offsetof(FixedPointIterSettings<T>, max_iter)
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}


template<std::floating_point T>
auto create_mpi_slab_type(MPI_Datatype grid_type) -> MPI_Datatype
{
    const int block_lengths[5] = {1, 1, 1, 1, 1};
    const MPI_Datatype types[5] = {
        grid_type,
        get_mpi_type<T>(),
        get_mpi_type<T>(),
        get_mpi_type<T>(),
        get_mpi_type<T>(),
    };

    const MPI_Aint offsets[5] = {
        offsetof(Slab<T>, grid),
        offsetof(Slab<T>, total_cross_section),
        offsetof(Slab<T>, scattering_cross_section),
        offsetof(Slab<T>, source),
        offsetof(Slab<T>, slab_width),
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(5, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}

template<std::floating_point T>
auto create_mpi_lab_type(MPI_Datatype slab_type, MPI_Datatype iter_type) -> MPI_Datatype
{
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    const MPI_Datatype types[n_fields] = {
        slab_type,
        iter_type,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Outlab08<T>, slab),
        offsetof(inlab08<T>, iter_settings),
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}


auto main(int argc, char* argv[]) -> int
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    auto grid_type = create_mpi_grid_type();
    auto iter_settings_type = create_mpi_iter_settings_type<real>();
    auto slab_type = create_mpi_slab_type<real>(grid_type);
    auto lab_type = create_mpi_lab_type<real>(slab_type, iter_settings_type);

    const Header header{.title = "NE 591 Outlab #08"};
    argparse::ArgumentParser program{
        "shumilov_outlab08",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("input").help("Path to input file.");
    program.add_argument("-o", "--output").help("Path to output file");

    try
    {

        Outlab08<real> lab{};
        if (get_local_rank(MPI_COMM_WORLD) == MANAGER_IDX)
        {
            program.parse_args(argc, argv);
            lab = read_input_file<Outlab08<real>>(program.get<std::string>("input"));

            if (lab.slab.grid.num_angular_points % get_world_size(MPI_COMM_WORLD) != 0)
            {

                throw std::runtime_error(
                    fmt::format(
                        "Number of processors must divide number of angular grid points: {} % {} != 0",
                        lab.slab.grid.num_angular_points,
                        get_world_size(MPI_COMM_WORLD)
                    )
                ); // Indicate an error occurred
            }

            MPI_Bcast(&lab, 1, lab_type, MANAGER_IDX, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Bcast(&lab, 1, lab_type, MANAGER_IDX, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);
        const auto solution = lab.run();
        MPI_Barrier(MPI_COMM_WORLD);

        if (get_local_rank(MPI_COMM_WORLD) == MANAGER_IDX)
        {
            if (const auto output_filename = program.present<std::string>("--output");
                output_filename.has_value())
            {
                std::ofstream output{ output_filename.value() };
                if (!output.is_open())
                {
                    throw std::runtime_error(
                        fmt::format("Could not open: '{}'", output_filename.value())
                    ); // Indicate an error occurred
                }

                header.echo(output);
                solution.echo(output);

                output.close();
            }
            else
            {
                header.echo(std::cout);
                solution.echo(std::cout);
            }
        }
    }
    catch (const std::exception& err)
    {
        fmt::print(
            std::cerr,
            "\n{}: {}\n\n",
            fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Error (Rank {}): ",
                get_local_rank(MPI_COMM_WORLD)
            ),
            err.what()
        );

        MPI_Abort(MPI_COMM_WORLD, 1);
        std::exit(EXIT_FAILURE);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
