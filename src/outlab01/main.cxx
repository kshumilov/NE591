#include <cstdlib>
#include <iostream>

#include <argparse/argparse.hpp>

#include <fmt/format.h>

#include "matrix.h"

/**
 * \brief Implementation of the Outlab01
 *
 * @param k Real number
 * @param M Row Rank of matrices A and B
 * @param N Column Rank of matrices A and B, row rank of matrix F
 * @param J Column rank of matrix F
 *
 * @var A - matrix of shape M x N
 * @var B - matrix of shape M x N
 * @var F - matrix of shape N x J
 * @var C - matrix of shape M x N
 */
auto outlab01(const double k, const std::size_t M, const std::size_t N, const std::size_t J) -> int {
  using namespace std::literals;

  Matrix<double> A {
    M, N,
    [](std::size_t m, std::size_t n) -> double {
      return m == n ? 1.0 : 0.5;
    }
  };
  A.display("A"sv, "where a_{m,n} = 1.0 if (m == n) else 0.5"sv);
  std::cout << "\n";

  Matrix<double> B {
    M, N,
    [](std::size_t m, std::size_t n) -> double {
      return m <= n ? 0.25 : 0.75;
    }
  };
  B.display("B"sv, "where b_{m,n} = 0.25 if (m <= n) else 0.75"sv);
  std::cout << "\n";

  Matrix<double> F {
    N, J,
    [](std::size_t n, std::size_t j) -> double {
      return 1 / static_cast<double>((n + 1) + (j + 1));
    }
  };
  F.display("F"sv, "where f_{n,j} = 1.0 / (n + j)"sv);
  std::cout << "\n";

  auto C { A + B };
  C.display("C"sv, "= A + B"sv);
  std::cout << "\n";

  auto D { k * A };
  D.display("D"sv, " = k * A"sv);
  std::cout << "\n";

  auto E { A * F };
  E.display("E"sv, "= A * F"sv);
  std::cout << "\n";

  return EXIT_SUCCESS;
}

// Example usage
auto main(int argc, char *argv[]) -> int {
  double k { 0.0 };
  int M { 3 };
  int N { 4 };
  int J { 5 };

  using namespace std::literals;
  argparse::ArgumentParser program(
    argv[0],
    "1.0",
    argparse::default_arguments::help
    );

  const std::string description {
    "================================================================================\n"s +
    fmt::format("{:<80s}\n", "NE 591 Outlab #01: Basic Math") +
    fmt::format("{:<80s}\n", "Author: Kirill Shumilov") +
    "================================================================================\n"s
    "This program showcases the use of matrix arithmetic,\n"s
    "such as matrix-matrix addition and multiplication and matrix-scalar\n"s
    "multiplication"s
  };

  program.add_description(description);

  program.add_argument("-k")
      .help("Scalar multiplier of matrix A (a real number)")
      .required()
      .scan<'g', double>()
      .store_into(k);

  program.add_argument("-M")
      .help("#rows for matrices A and B (positive integer)")
      .required()
      .scan<'d', int>()
      .store_into(M);

  program.add_argument("-N")
      .help("#rows for matrix F,\n"
            "#cols for matrices A and B (positive integer)")
      .required()
      .scan<'d', int>()
      .store_into(N);

  program.add_argument("-J")
      .help("#cols for matrix F (positive integer)")
      .required()
      .scan<'d', int>()
      .store_into(J);

  try {
    program.parse_args(argc, argv);

    if (M <= 0 || N <= 0 || J <= 0) {
      throw std::invalid_argument("Integers M, N, J must be positive");
    }
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(EXIT_FAILURE);
  }

  std::cout << description << "\n" <<
  "--------------------------------------------------------------------------------\n"s +
  "Input Arguments:\n" +
  fmt::format("k = {}\n", k) +
  fmt::format("M = {}\n", M) +
  fmt::format("N = {}\n", N) +
  fmt::format("J = {}\n", J) +
  "--------------------------------------------------------------------------------\n"s;

  return outlab01(
    k,
    static_cast<size_t>(M),
    static_cast<size_t>(N),
    static_cast<size_t>(J)
    );
}
