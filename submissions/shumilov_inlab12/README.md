# Outlab #11
- Author: Kirill Shumilov
- Date: 04/04/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
```
shumilov_inlab12/
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
    └── inlab12.cpp
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
[kshumil@login03 shumilov_inlab12]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_inlab12/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_inlab12/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build -- -j
```

### Example
```bash
[kshumil@login02 shumilov_inlab12]$ cmake --build build -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/inlab12.dir/src/inlab12.cpp.o
[100%] Linking CXX executable shumilov_inlab12
[100%] Built target inlab12
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
[kshumil@login03 shumilov_inlab12]$ ./shumilov_inlab12 examples/symm2.inp
================================================================================
NE 501 Outlab #11
Author: Kirill Shumilov
Date: 04/04/2025
--------------------------------------------------------------------------------
Power Iteration for finding fundamental eigenvalue and corresponding eigenvector
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Tolerance: 0.0001
Maximum #Iterations: 10000
Matrix A, <4 x 4, e>:
[[ 4.00000000e+00 -1.00000000e+00 -1.00000000e+00  0.00000000e+00]
 [-1.00000000e+00  3.00000000e+00  0.00000000e+00 -1.00000000e+00]
 [-1.00000000e+00  0.00000000e+00  4.00000000e+00 -1.00000000e+00]
 [ 0.00000000e+00 -1.00000000e+00 -1.00000000e+00  3.00000000e+00]]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged: true
# Iterations: 22
Error: 8.19813794e-05
Eigenvalue: 5.61803397e+00
Eigenvector:
 1.00000000e+00 -6.18021901e-01 -9.99875106e-01  6.17944712e-01
================================================================================
Execution time: 00.000002000 seconds.
================================================================================
```

### Printing to file
```bash
[kshumil@login03 shumilov_inlab12]$ ./shumilov_inlab12 examples/symm_rayleigh.inp -o symm_rayleigh.out
[kshumil@login03 shumilov_inlab12]$ cat symm_rayleigh.out 
================================================================================
NE 501 Inlab #12
Author: Kirill Shumilov
Date: 04/04/2025
--------------------------------------------------------------------------------
Power iteration with Rayleigh Quotien
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
Eigenvalue update policy: Rayleigh Quotient
Eigenvalue guess: 3.00000000e+00
Eigenvector guess, x:
[ 1.00000000e+00,  0.00000000e+00,  0.00000000e+00,  0.00000000e+00]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged: true
# Iterations: 27
Max Abs Residual:  1.47914089e-05
Residual:
-1.47914089e-05 7.00469837e-06 -1.28805793e-05 1.00975309e-05
Eigenvalue Error:  5.88948890e-11
Eigenvalue: 5.61803399e+00
Eigenvector Error: 8.67081390e-06
Eigenvector:
 1.00000000e+00 -6.18033033e-01 -9.99986164e-01  6.18024482e-01
================================================================================
Execution time: 00.000003000 seconds.
================================================================================
```