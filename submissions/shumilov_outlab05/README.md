# Outlab #05
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
shumilov_outlab05  <-- Root Directory (Run commands from this directory)
├── CMakeLists.txt
├── README.md
├── examples  <--- Example of Input Files
│   ├── test1.inp
│   ├── test2.inp
│   └── test3.inp
├── include
│   └── methods
│       ├── array.h    <--- Implementations of array operations such as max abs element
│       ├── linalg
│       │   ├── blas.h   <--- Local implementation of GEMV, GEMM, etc.
│       │   ├── lu.h     <--- LU Factorization with/without Pivoting
│       │   ├── matrix.h  <--- LinAlg Matrix Operations Abastraction
│       │   ├── vec.h  <--- LinAlg Vector Operations Abstractions
│       │   └── utils  
│       │       └── io.h  <--- IO for Matrices
│       └── utils
│           ├── io.h  <--- General helper IO
│           └── math.h  <--- isclose definition
└── src
    └── main.cxx  <<--- Program Entrypoint

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
[kshumil@login02 shumilov_outlab05]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_outlab05/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_outlab05/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target outlab05 -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab05]$ cmake --build build --config Release --target outlab05 -- -j
[ 20%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/outlab05.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_outlab05
[100%] Built target outlab05
```

At this point the executable can be found in:
```bash
[kshumil@login02 shumilov_outlab05]$ ./shumilov_outlab05 -h
================================================================================
NE 591 Outlab #05: Solution of Ax=b using LUP Factorization Result
Author: Kirill Shumilov
Date: 02/07/2025
--------------------------------------------------------------------------------
This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,
using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)
================================================================================
Usage: shumilov_inlab05 [--help] filename

This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,
using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)


Positional arguments:
  filename    Path to input file

Optional arguments:
  -h, --help  shows help message and exits
```

# Examples
## Successful LUP Factorization and Solution
```bash
[kshumil@login02 shumilov_outlab05]$ cat examples/test3.inp
5

1.00 2.00 3.00 4.00 5.00
2.00 3.00 4.00 5.00 1.00
3.00 4.00 5.00 1.00 2.00
4.00 5.00 1.00 2.00 3.00
5.00 1.00 2.00 3.00 4.00

1.10 1.20 1.30 1.40 1.50

1
[kshumil@login02 shumilov_outlab05]$ ./shumilov_outlab05 examples/test3.inp
================================================================================
NE 591 Inlab #05: Solution of Ax=b using LUP Factorization Result
Author: Kirill Shumilov
Date: 02/07/2025
--------------------------------------------------------------------------------
This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,
using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)
================================================================================
                                     Inputs
--------------------------------------------------------------------------------
Original Matrix, A<5 x 5, e>:
[[ 1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00]
 [ 2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00]
 [ 3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00]
 [ 4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00]
 [ 5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00]]

RHS vector, b:
[ 1.100000e+00  1.200000e+00  1.300000e+00  1.400000e+00  1.500000e+00]

Pivoting Method: Partial Pivoting
================================================================================
                                    Results
--------------------------------------------------------------------------------
Lower Triangular Matrix, L<5 x 5, e>:
[[ 1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 8.000000e-01  1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 6.000000e-01  8.095238e-01  1.000000e+00  0.000000e+00  0.000000e+00]
 [ 4.000000e-01  6.190476e-01  8.333333e-01  1.000000e+00  0.000000e+00]
 [ 2.000000e-01  4.285714e-01  6.666667e-01  8.750000e-01  1.000000e+00]]

Upper Triangular Matrix, U<5 x 5, e>:
[[ 5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00]
 [ 0.000000e+00  4.200000e+00 -6.000000e-01 -4.000000e-01 -2.000000e-01]
 [ 0.000000e+00  0.000000e+00  4.285714e+00 -4.761905e-01 -2.380952e-01]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  4.444444e+00 -2.777778e-01]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  4.687500e+00]]

Permutation Matrix, P<5 x 5, e>:
[[ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  1.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  1.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  1.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]]

Matrix Residual, R<5 x 5, e> = L * U - P * A,
with norm |R| =  0.000000e+00 and max|Rij| =  0.000000e+00:
[[ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]]
--------------------------------------------------------------------------------
Solution Vector, x:
[ 1.666667e-01  6.666667e-02  6.666667e-02  6.666667e-02  6.666667e-02]

Solution Residual, r = b - A * x,
with norm |r| =  0.000000e+00 and max|ri| =  8.131516e-20:
[ 0.000000e+00 -2.710505e-20  2.710505e-20  0.000000e+00 -8.131516e-20]
================================================================================
```

## Failed LU Factorization (small pivots)
Notice an issued warning:
```bash
[kshumil@login02 shumilov_outlab05]$ ./shumilov_outlab05 examples/test4.inp
================================================================================
NE 591 Inlab #05: Solution of Ax=b using LUP Factorization Result
Author: Kirill Shumilov
Date: 02/07/2025
--------------------------------------------------------------------------------
This program solves system of equations Ax=b
using LUP Factorization, PAx=LUx=Pb,
using forward and backward substitution based on
lower triangular matrix (L), upper-triangular matrix (U),
row permutation matrix (P), and RHS vector (b)
================================================================================
                                     Inputs
--------------------------------------------------------------------------------
Original Matrix, A<5 x 5, e>:
[[ 1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00]
 [ 2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00]
 [ 3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00]
 [ 4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00]
 [ 5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00]]

RHS vector, b:
[ 1.100000e+00  1.200000e+00  1.300000e+00  1.400000e+00  1.500000e+00]

Pivoting Method: No Pivoting
================================================================================
Error: Small Pivot Encountered
                                    Results
--------------------------------------------------------------------------------
Lower Triangular Matrix, L<5 x 5, e>:
[[ 1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 2.000000e+00  1.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]
 [ 3.000000e+00  2.000000e+00  1.000000e+00  0.000000e+00  0.000000e+00]
 [ 4.000000e+00  3.000000e+00 -inf          1.000000e+00  0.000000e+00]
 [ 5.000000e+00  9.000000e+00  inf         -nan          1.000000e+00]]

Upper Triangular Matrix, U<5 x 5, e>:
[[ 1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00]
 [ 0.000000e+00 -1.000000e+00 -2.000000e+00 -3.000000e+00 -9.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00 -5.000000e+00  5.000000e+00]
 [ 0.000000e+00  0.000000e+00  0.000000e+00 -inf          inf        ]
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00 -nan        ]]

Matrix Residual, R<5 x 5, e> = L * U - P * A,
with norm |R| = -nan         and max|Rij| =  0.000000e+00:
[[ 0.000000e+00  0.000000e+00  0.000000e+00 -nan         -nan        ]
 [ 0.000000e+00  0.000000e+00  0.000000e+00 -nan         -nan        ]
 [ 0.000000e+00  0.000000e+00  0.000000e+00 -nan         -nan        ]
 [-nan         -nan         -nan         -nan         -nan        ]
 [-nan         -nan         -nan         -nan         -nan        ]]
--------------------------------------------------------------------------------
Solution Vector, x:
[-nan         -nan         -nan         -nan         -nan        ]

Solution Residual, r = b - A * x,
with norm |r| = -nan         and max|ri| =  nan        :
[-nan         -nan         -nan         -nan         -nan        ]
================================================================================
```
