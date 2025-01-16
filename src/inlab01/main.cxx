#include <cmath>
#include <iostream>
#include <iomanip>

#include <argparse/argparse.hpp>

// Structure to hold the result of Truncated Taylor Series Approximation Expansion
struct Result {
    double value;  // Estimated value of f(x)
    double Tn;     // nth Last calculated term of Taylor series expansion
    int n;         // Index of last calculated term in Taylor Series Expansion
};

/*!
 * \brief Calculates Truncated Taylor Series Expansion (TTSE) for sin(x) around x0 = 0
 *
 * @param x Value to evaulate sin(x) at, must be |x| < 1.0, no checking is done here;
 * @param eps Maximum absolute value of a TTSE term to be added to expansion;
 * @param max_terms Maximum number of terms to be added to the expansion.
 *
 * Variable Declaration Document:
 *   - result - struct to hold results of intermediate calculation (see Result struct definition)
 *   - Tn_plus_1 - (n + 1)th TTSE term, wrt to Tn
 *
 * @return TTSE of sin(x) where all terms are either < eps or the
 *         total number of terms < max_terms, whichever happens first.
 *         First value of the pair is result-struct, second value is the boolean flag,
 *         indicating if the calculation converged
 */
auto taylor_sin(double x, double eps, int max_terms) -> std::pair<Result, bool> {
    // Initialize the calculation state
    auto result = Result{x, x, 0};

    // Iterate until max number of allowed terms is reached
    for (size_t i = 0; i < max_terms; i++) {
        // Calculate the next T(n+1) from Tn
        auto Tn_plus_1 = (-1) * x * x * result.Tn / ((2 * result.n + 2) * (2 * result.n + 3));

        // Update the state
        result.n += 1;
        result.Tn = Tn_plus_1;
        result.value += Tn_plus_1;

        // Check if threshold is reached
        if (abs(Tn_plus_1) < eps) {
            return std::make_pair(result, true);
        }
    }

    // If convergence is not reached, return the state and notify
    return std::make_pair(result, false);
}

/*!
 * \brief Parses command line arguments supplied to the program and validates them.
 *
 * In case of validation error, prints the error message and terminates the program.
 *
 * @param argc number of arguments passed in command line
 * @param argv pointer to array of strings, holding cmd arguments
 *
 * Variable Declaration Document:
 *   - desc - object, holding information about input options to the program
 *   - vm - key-value map, holding results of parsing
 *
 * @return values of x, eps, and max_terms
 */
auto parse_and_validate(int argc, char* argv[]) -> std::tuple<double, double, int>  {

    // Variables to store the command-line arguments
    double x{0.0};
    double eps{1e-6};
    int max_terms{100};

    argparse::ArgumentParser program(
      argv[0],
      "1.0",
      argparse::default_arguments::help
      );

    // Define options
    po::options_description desc("Allowed options");
    desc.add_options()
        (
            "angle,x",
            po::value<double>(&x)->required(),
            "A value to calculate sin(x) at. Must satisfy |x| < 1.0")
        (
            "max_terms,n",
            po::value<int>(&max_terms)->required(),
            "Maximum number of terms. Must satisfy n > 1")
        (
            "eps,e",
            po::value<double>(&eps)->required(),
            "Stopping condition. Must satisfy e > 0.0");

    // Parse options
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        // Check for required arguments
        po::notify(vm);

        if (std::abs(x) >= 1.0) {
            std::cerr << "Error: x must be a real value that satisfies |x| < 1.0 " << std::endl;
            throw po::validation_error(po::validation_error::invalid_option_value, "x");
        }

        // Validate constraints
        if (max_terms <= 0) {
            std::cerr << "Error: n must be greater than 1" << std::endl;
            throw po::validation_error(po::validation_error::invalid_option_value, "n");
        }

        if (eps <= 0.0) {
            std::cerr << "Error: threshold must be greater than 0.0" << std::endl;
            throw po::validation_error(po::validation_error::invalid_option_value, "e");
        }
    } catch (po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << desc << "\n";
        std::exit(EXIT_FAILURE);
    }

    return std::make_tuple(x, eps, max_terms);
}

/**
 * \brief Runs IO of the progam.
 *
 * @param argc number of arguments passed in command line
 * @param argv pointer to array of strings, holding cmd arguments
 *
 * Variable Declaration Doc:
 *   - x - Value to evaulate sin(x) at, must be |x| < 1.0, no checking is done here;
 *   - eps - Maximum absolute value of a TTSE term to be added to expansion;
 *   - max_terms - Maximum number of terms to be added to the expansion.
 *   - result - result of calculation
 *   - coverged - boolean flag, indicating if calculation has converged
 *   - true_value - value of sin(x), evaluated with std::sin - intrinsic function
 *   - error - difference between TTSE and true_value
 *
 * @return 0
 */
auto main(int argc, char* argv[]) -> int {
    std::cout << "================================================================================" << std::endl
              << "InLab 01, Basic Math" << std::endl
              << "This program was originally developed by Kirill Shumilov " << std::endl
              << "for NE 591 (Fri Jan10, Spring 2025, NSCU)" << std::endl
              << "--------------------------------------------------------------------------------" << std::endl
              << "The purpose of this program is to calculate the value of sin(x)" << std::endl
              << "using Truncated Taylor Series Approximation around x0 = 0.0" << std::endl;
    std::cout << "================================================================================" << std::endl;

    // Read the parameters
    auto [x, eps, max_terms] = parse_and_validate(argc, argv);

    // Echo back input variables
    std::cout << "Input Data:" << std::endl
              << "--------------------------------------------------------------------------------" << std::endl
              << std::setprecision(6) << std::scientific
              << "Angles (radians)   = " << x << std::endl
              << "Stopping Criterion = " << eps << std::endl
              << "Max Series index   = " << max_terms << std::endl
              << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "All input data is valid!" << std::endl;

    // Perform the calculation
    auto [result, converged] = taylor_sin(x, eps, max_terms);

    // Calculate the error in the estimate
    const auto true_value = std::sin(x);
    const auto error = true_value - result.value;

    // Output the result
    std::cout << "================================================================================" << std::endl;
    std::cout << "Results:" << std::endl
              << "--------------------------------------------------------------------------------" << std::endl
              << std::boolalpha
              << "Convergence is achieved          = " << converged << std::endl
              << "Series Index reached             = " << result.n << std::endl
              << "--------------------------------------------------------------------------------" << std::endl
              << std::setprecision(6) << std::scientific
              << "sin(x) (w/ Taylor Series Approx) = " << result.value << std::endl
              << "sin(x) (w/ Intrinsic Func)       = " << true_value << std::endl
              << "Approximation Error              = " << error << std::endl;
    std::cout << "================================================================================" << std::endl;
    return 0;
}