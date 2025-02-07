# Outlab #04
- Author: Kirill Shumilov
- Date: 02/07/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.26
```

# File Tree
The project `main.cxx` function is located under `src`
```
shumilov_outlab04  <-- Root Directory (Run commands from this directory)
├── CMakeLists.txt    <-- Compilation Script (do not edit)
├── README.md   <-- Instructions
├── include
│   ├── linalg
│   │   ├── matrix.h  <-- Matrix Class implementation
│   │   └─── lu.h   <-- LU Forward and Backward Substitution Implementation
└── src
    └── main.cxx  <-- Entry point
```

# Hazel HPC System (NCSU)
The code has been run and tested on Hazel. Before continuing login in on the login node of the cluster:
```bash
ssh -X $USER:login.hpc.ncsu.edu
```
Load the latest gcc compile and CMake:
```bash
module load gcc/13.2.0
module load cmake/3.24.1
```

## 1. Initialize the build directory
```bash
cmake -S. -Bbuild
```
### Example
```bash
[kshumil@login02 shumilov_outlab04]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_outlab04/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target outlab04 -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab04]$ cmake --build build --config Release --target outlab04 -- -j
[ 50%] Building CXX object CMakeFiles/outlab04.dir/src/main.cxx.o
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/src/main.cxx: In lambda function:
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/src/main.cxx:161:37: warning: declaration of ‘const auto:91 b’ shadows a parameter [-Wshadow]
  161 |         [](const auto a, const auto b) {
      |                          ~~~~~~~~~~~^
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/src/main.cxx:118:67: note: shadowed declaration is here
  118 | void outlab04(const Matrix<double>& A, const std::vector<double>& b)
      |                                        ~~~~~~~~~~~~~~~~~~~~~~~~~~~^
In file included from /home/kshumil/ne591/NE591/submissions/shumilov_outlab04/src/main.cxx:21:
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/include/linalg/lu.h: In instantiation of ‘std::vector<scalar_t> backward_substitution(const Matrix<T>&, std::span<const scalar_t>) [with scalar_t = double]’:
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/include/linalg/lu.h:130:43:   required from ‘std::vector<scalar_t> solve_lu(const Matrix<T>&, const Matrix<T>&, std::span<const scalar_t>) [with scalar_t = double; bool LowerUnitDiag = true]’
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/src/main.cxx:150:42:   required from here
/home/kshumil/ne591/NE591/submissions/shumilov_outlab04/include/linalg/lu.h:114:41: warning: comparison of integer expressions of different signedness: ‘std::ptrdiff_t’ {aka ‘long int’} and ‘Matrix<double>::idx_t’ {aka ‘long unsigned int’} [-Wsign-compare]
  114 |         for (std::ptrdiff_t j{r + 1}; j < U.cols(); ++j) {
      |                                       ~~^~~~~~~~~~
[100%] Linking CXX executable shumilov_outlab04
[100%] Built target outlab04
```

At this point the executable can be found in:
```bash
[kshumil@login02 shumilov_outlab04]$ ./shumilov_outlab04 -h
Usage: shumilov_outlab04 [--help] [--version] filename

This program solves system of equations Ax=LUx=b
by performing LU factorization of A and then
using forward and backward substitution based on
lower triangular (L), upper-triangular (U), and RHS vector (b)

Positional arguments:
  filename       Path to input fle

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
```

# Examples
```bash
[kshumil@login02 shumilov_outlab04]$ cat examples/inp
3

 2 -1 -2
-4  6  3
-4 -2  8

-6 17 16
[kshumil@login02 shumilov_outlab04]$ ./shumilov_outlab04 examples/inp
================================================================================
NE 591 Outlab #04: Solution of Ax=b using LU Factorization
Author: Kirill Shumilov
Date: 01/31/2025
================================================================================
                                     Inputs
--------------------------------------------------------------------------------
Original Matrix, A:
[[ 2.000000e+00 -1.000000e+00 -2.000000e+00]
 [-4.000000e+00  6.000000e+00  3.000000e+00]
 [-4.000000e+00 -2.000000e+00  8.000000e+00]]

RHS vector, b:
[   -6.000000    17.000000    16.000000]
================================================================================
                                    Results
--------------------------------------------------------------------------------
Lower Triangular Matrix, L:
[[ 1.000000e+00  0.000000e+00  0.000000e+00]
 [-2.000000e+00  1.000000e+00  0.000000e+00]
 [-2.000000e+00 -1.000000e+00  1.000000e+00]]

Upper Triangular Matrix, L:
[[ 2.000000e+00 -1.000000e+00 -2.000000e+00]
 [ 0.000000e+00  4.000000e+00 -1.000000e+00]
 [ 0.000000e+00  0.000000e+00  3.000000e+00]]

Matrix Residual, R = LU - A, with norm |R| =  0.000000e+00
[[ 0.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00]]
--------------------------------------------------------------------------------
Solution vector, x:
[    1.000000     2.000000     3.000000]
--------------------------------------------------------------------------------
Max Element of residual vector, r = b - Ax:            0
================================================================================
```
