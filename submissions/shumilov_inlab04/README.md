# Inlab #04
- Author: Kirill Shumilov
- Date: 01/31/2024

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.26
```

# File Tree
The project `main.cxx` function is located under `src`
```
shumilov_inlab04  <-- Root Directory (Run commands from this directory)
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
Load the latest gcc compiler:
```bash
module load gcc/13.2.0
```

## 1. Initialize the build directory
```bash
cmake -S. -Bbuild
```
### Example
```bash
[kshumil@login03 shumilov_inlab04]$ module load gcc/13.2.0
[kshumil@login03 shumilov_inlab04]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (4.8s)
-- Generating done (0.1s)
-- Build files have been written to: /home/kshumil/shumilov_inlab04/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target inlab04 -- -j
```

### Example
```bash
[kshumil@login02 shumilov_inlab04]$ cmake --build build --config Release --target inlab04 -- -j
[ 50%] Building CXX object CMakeFiles/inlab04.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_inlab04
[100%] Built target inlab04
```

At this point the executable can be found in:
```bash
[kshumil@login03 shumilov_inlab04]$ cd ./shumilov_inlab04 -h
================================================================================
NE 591 Inlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/24/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
Usage: shumilov_inlab04 [--help] --start VAR --end VAR --subintervals VAR [--quad VAR]

Optional arguments:
  -h, --help          shows help message and exits
  -a, --start         Start of interval integration [required]
  -b, --end           End of interval integration [required]
  -m, --subintervals  Number of subintervals [required]
  --quad              Quadrature type (trap, simp, gauss) nargs=0..1] [default: "trap"]
```

# Examples
```bash
[kshumil@login03 inlab04]$ ./shumilov_inlab04
================================================================================
NE 591 Inlab #04: Solution State of LU Factorization
Author: Kirill Shumilov
Date: 01/31/2025
================================================================================
This program performs solves system of equations Ax=LUx=b
using forward and backward substitution based on
lower triangular (L), upper-triangular (U), and RHS vector (b)
================================================================================
Enter matrix rank:
sdf

Error: Invalid Input, pleas enter a valid number
asdf

Error: Invalid Input, pleas enter a valid number
3
Enter non-zero values of lower triangular matrix in row-major order:
1
2 1
3 2 1
Enter non-zero values of upper triangular matrix in row-major order:
1 1 2
  2 3
    3
Enter RHS vector:
7 21 38
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Lower triangular matrix: <3 x 3, d> 
[[  1.00   0.00   0.00] 
 [  2.00   1.00   0.00] 
 [  3.00   2.00   1.00]]
--------------------------------------------------------------------------------
Upper triangular matrix: <3 x 3, d> 
[[  1.00   1.00   2.00] 
 [  0.00   2.00   3.00] 
 [  0.00   0.00   3.00]]
--------------------------------------------------------------------------------
Original Matrix: <3 x 3, d> A = L * U
[[  1.00   1.00   2.00] 
 [  2.00   4.00   7.00] 
 [  3.00   7.00  15.00]]
--------------------------------------------------------------------------------
RHS Vector, b:
    7.000000    21.000000    38.000000
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Solution vector, x:
    3.000000     2.000000     1.000000
================================================================================
```
