# Outlab #04
- Author: Kirill Shumilov
- Date: 02/07/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
The project `main.cxx` function is located under `src`
```
shumilov_inlab05  <-- Root Directory (Run commands from this directory)
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
[kshumil@login02 shumilov_inlab05]$ cmake -S. -Bbuild
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
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_inlab05/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target inlab05 -- -j
```

### Example
```bash
[kshumil@login02 shumilov_inlab05]$ cmake --build build --config Release --target inlab05 -- -j
[ 50%] Building CXX object CMakeFiles/inlab05.dir/src/main.cxx.o
In file included from /home/kshumil/ne591/NE591/submissions/shumilov_inlab05/src/main.cxx:17:
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/include/linalg/lu.h: In instantiation of ‘std::vector<scalar_t> backward_substitution(const Matrix<T>&, std::span<const scalar_t>) [with scalar_t = double]’:
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/include/linalg/lu.h:142:43:   required from ‘constexpr std::vector<scalar_t> solve_lu(const Matrix<T>&, const Matrix<T>&, std::span<const scalar_t>) [with scalar_t = double; Diag UnitLower = Diag::NonUnit]’
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/include/linalg/lu.h:150:41:   required from ‘std::vector<scalar_t> solve_lup(const Matrix<T>&, const Matrix<T>&, const Matrix<T>&, std::span<const scalar_t>) [with scalar_t = double; Diag UnitLower = Diag::NonUnit]’
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/src/main.cxx:63:37:   required from ‘constexpr Inlab05<scalar_t>::Result Inlab05<scalar_t>::solve() const [with scalar_t = double]’
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/src/main.cxx:72:29:   required from ‘Inlab05<scalar_t>::Result Inlab05<scalar_t>::run() const [with scalar_t = double]’
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/src/main.cxx:146:20:   required from here
/home/kshumil/ne591/NE591/submissions/shumilov_inlab05/include/linalg/lu.h:122:41: warning: comparison of integer expressions of different signedness: ‘std::ptrdiff_t’ {aka ‘long int’} and ‘Matrix<double>::idx_t’ {aka ‘long unsigned int’} [-Wsign-compare]
  122 |         for (std::ptrdiff_t j{r + 1}; j < U.cols(); ++j) {
      |                                       ~~^~~~~~~~~~
[100%] Linking CXX executable shumilov_inlab05
[100%] Built target inlab05
```

At this point the executable can be found in:
```bash
[kshumil@login02 shumilov_inlab05]$ ./shumilov_inlab05 -h
================================================================================
NE 591 Inlab #05: Solution of Ax=b using LUP Factorization Result
Author: Kirill Shumilov
Date: 02/07/2025
================================================================================
This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)
================================================================================
Usage: shumilov_inlab05 [--help] [--version] filename

This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)


Positional arguments:
  filename       Path to input file 

Optional arguments:
  -h, --help     shows help message and exits 
  -v, --version  prints version information and exits
```

# Examples
```bash
[kshumil@login02 shumilov_inlab05]$ cat examples/test1.txt
5

1.00 0.00 0.00 0.00 0.00
0.80 1.00 0.00 0.00 0.00
0.60 0.80 1.00 0.00 0.00
0.40 0.62 0.83 1.00 0.00
0.20 0.43 0.67 0.88 1.00

5.00 1.00 2.00 3.00 4.00
0.00 4.20 -.60 -.40 -.20
0.00 0.00 4.29 -.48 -.24
0.00 0.00 0.00 4.44 -.28
0.00 0.00 0.00 0.00 4.69

0.00 0.00 0.00 0.00 1.00
0.00 0.00 0.00 1.00 0.00
0.00 0.00 1.00 0.00 0.00
0.00 1.00 0.00 0.00 0.00
1.00 0.00 0.00 0.00 0.00

1.10 1.20 1.30 1.40 1.50
[kshumil@login02 shumilov_inlab05]$ ./shumilov_inlab05 examples/test1.txt
================================================================================
NE 591 Inlab #05: Solution of Ax=b using LUP Factorization Result
Author: Kirill Shumilov
Date: 02/07/2025
================================================================================
This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Lower Triangular Matrix, L:
[[ 1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 8.000000e-01  1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 6.000000e-01  8.000000e-01  1.000000e+00  0.000000e+00  0.000000e+00] 
 [ 4.000000e-01  6.200000e-01  8.300000e-01  1.000000e+00  0.000000e+00] 
 [ 2.000000e-01  4.300000e-01  6.700000e-01  8.800000e-01  1.000000e+00]]

Upper Triangular Matrix, L:
[[ 5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00] 
 [ 0.000000e+00  4.200000e+00 -6.000000e-01 -4.000000e-01 -2.000000e-01] 
 [ 0.000000e+00  0.000000e+00  4.290000e+00 -4.800000e-01 -2.400000e-01] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  4.440000e+00 -2.800000e-01] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  4.690000e+00]]

Permutation Matrix, P:
[[ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  1.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  1.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  1.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]]

RHS vector, b:
[ 1.100000e+00  1.200000e+00  1.300000e+00  1.400000e+00  1.500000e+00]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Solution Vector, x:
[ 1.671165e-01  6.667917e-02  6.707845e-02  6.650553e-02  6.601620e-02]
================================================================================
```
