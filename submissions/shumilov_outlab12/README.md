# Outlab #11
- Author: Kirill Shumilov
- Date: 04/11/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
```
shumilov_outlab12/
├── CMakeLists.txt
├── README.md
├── examples
│   ├── asymm.inp
│   ├── asymm_err.inp
│   ├── symm.inp
│   ├── symm_err.inp
│   ├── symm_point.inp
│   └── symm_rayleigh.inp
├── include
│   ├── CMakeLists.txt
│   ├── lab
│   │   ├── config.h
│   │   ├── io.h
│   │   └── lab.h
│   ├── methods
│   │   ├── array.h
│   │   ├── fixed_point
│   │   │   ├── algorithm.h
│   │   │   ├── settings.h
│   │   │   └── state.h
│   │   ├── fixed_point.h
│   │   ├── linalg
│   │   │   ├── blas.h
│   │   │   ├── eig
│   │   │   │   ├── eig_update.h
│   │   │   │   ├── inverse_shift.h
│   │   │   │   └── power_iter.h
│   │   │   ├── lu.h
│   │   │   ├── math.h
│   │   │   ├── matrix.h
│   │   │   ├── utils
│   │   │   │   ├── io.h
│   │   │   │   └── math.h
│   │   │   └── vec.h
│   │   ├── math.h
│   │   └── matrix.h
│   └── utils
│       └── io.h
├── lab_include
│   └── lab12.h
└── src
    └── outlab12.cpp
p
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
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
```
### Example
```bash
[kshumil@login03 shumilov_outlab12]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_outlab12/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_outlab12/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab12]$ cmake --build build -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/outlab12.dir/src/outlab12.cpp.o
[100%] Linking CXX executable shumilov_outlab12
[100%] Built target outlab12
```

At this point the executable can be found in:
```bash
Usage: shumilov_outlab11 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file. 

Optional arguments:
  -h, --help    shows help message and exits 
  -o, --output  Path to output file 
```

## 3. Examples
### Printing to stdout
```bash
[kshumil@login03 shumilov_outlab12]$ ./shumilov_outlab12 examples/symm_point.inp
================================================================================
NE 591 Outlab #12
Author: Kirill Shumilov
Date: 04/11/2025
--------------------------------------------------------------------------------
Inverse-shift iteration to solve Ax=lx problem
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Tolerance: 1e-05
Maximum #Iterations: 10000
Matrix A, <4 x 4, e>:
[[ 4.00000000e+00 -1.00000000e+00 -1.00000000e+00  0.00000000e+00]
 [-1.00000000e+00  3.00000000e+00  0.00000000e+00 -1.00000000e+00]
 [-1.00000000e+00  0.00000000e+00  4.00000000e+00 -1.00000000e+00]
 [ 0.00000000e+00 -1.00000000e+00 -1.00000000e+00  3.00000000e+00]]
Eigenvalue update policy: Power Iteration
Eigenvalue guess: 1.00000000e+00
Eigenvector guess, x:
[ 1.00000000e+00,  0.00000000e+00,  0.00000000e+00,  0.00000000e+00]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged: true
# Iterations: 8
Iterative Error: 4.53019895e-06
Max Abs Residual: 1.73037452e-06
Residual:
 1.04726368e-06  0.00000000e+00 -6.36637641e-08 -1.73037452e-06
Eigenvalue Iter Error: 1.29410032e-06
Eigenvalue: 1.38196642e+00
Eigenvector Iter Error: 4.53019895e-06
Eigenvector:
 6.18034453e-01  1.00000000e+00  6.18033905e-01  9.99999130e-01
================================================================================
Execution time: 00.000080000 seconds.
================================================================================

```

### Printing to file
```bash
[kshumil@login03 shumilov_outlab12]$ ./shumilov_outlab12 examples/symm_rayleigh.inp -o symm_rayleigh.out
[kshumil@login03 shumilov_outlab12]$ cat symm_rayleigh.out 
================================================================================
NE 591 Outlab #12
Author: Kirill Shumilov
Date: 04/11/2025
--------------------------------------------------------------------------------
Inverse-shift iteration to solve Ax=lx problem
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Tolerance: 0.0001
Maximum #Iterations: 30
Matrix A, <4 x 4, e>:
[[ 4.00000000e+00 -1.00000000e+00 -1.00000000e+00  0.00000000e+00]
 [-1.00000000e+00  3.00000000e+00  0.00000000e+00 -1.00000000e+00]
 [-1.00000000e+00  0.00000000e+00  4.00000000e+00 -1.00000000e+00]
 [ 0.00000000e+00 -1.00000000e+00 -1.00000000e+00  3.00000000e+00]]
Eigenvalue update policy: Rayleigh Quotient
Eigenvalue guess: 3.00000000e+00
Eigenvector guess, x:
[ 1.00000000e+00,  0.00000000e+00,  0.00000000e+00,  0.00000000e+00]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged: true
# Iterations: 21
Iterative Error: 9.13696547e-05
Max Abs Residual: 1.56057367e-05
Residual:
 1.56044615e-05 -9.64551307e-06  1.56057367e-05 -9.64344986e-06
Eigenvalue Iter Error: 4.93570962e-10
Eigenvalue: 3.38196601e+00
Eigenvector Iter Error: 9.13696547e-05
Eigenvector:
 6.18074843e-01  9.99918291e-01 -6.17942635e-01 -1.00000000e+00
================================================================================
Execution time: 00.000140000 seconds.
================================================================================

```