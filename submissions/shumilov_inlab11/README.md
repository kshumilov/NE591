# Inlab #11
- Author: Kirill Shumilov
- Date: 03/28/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
```
├── CMakeLists.txt
├── README.md
├── examples
│   ├── asymm.inp
│   ├── symm.inp
│   └── symm2.inp
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
│   │   │   ├── Axb
│   │   │   │   ├── algorithm.h
│   │   │   │   ├── cg.h
│   │   │   │   ├── linear_system.h
│   │   │   │   ├── pcg.h
│   │   │   │   ├── sor.h
│   │   │   │   └── state.h
│   │   │   ├── blas.h
│   │   │   ├── lu.h
│   │   │   ├── matrix.h
│   │   │   ├── utils
│   │   │   │   ├── io.h
│   │   │   │   └── math.h
│   │   │   └── vec.h
│   │   └── math.h
│   └── utils
│       └── io.h
├── lab_include
│   ├── build_system.h
│   └── lab11.h
└── src
    └── inlab11.cpp

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
[kshumil@login03 shumilov_inlab11]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_inlab11/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_inlab11/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build -- -j
```

### Example
```bash
[kshumil@login02 shumilov_inlab11]$ cmake --build build -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/inlab11.dir/src/inlab11.cpp.o
[100%] Linking CXX executable shumilov_inlab11
[100%] Built target inlab11
```

At this point the executable can be found in:
```bash
sage: shumilov_outlab11 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file. 

Optional arguments:
  -h, --help    shows help message and exits 
  -o, --output  Path to output file 
```

## 3. Examples
## Printing to stdout
```bash
================================================================================
NE 501 Inlab #11
Author: Kirill Shumilov
Date: 03/28/2025
--------------------------------------------------------------------------------
Implementation of PCG solver with Jacobi Preconditioner for Ax=b systems
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Tolerance: 0.0001
Maximum #Iterations: 10
Matrix, A: <4 x 4, e>:
[[ 4.00000000e+00 -1.00000000e+00 -1.00000000e+00  0.00000000e+00]
 [-1.00000000e+00  3.00000000e+00  0.00000000e+00 -1.00000000e+00]
 [-1.00000000e+00  0.00000000e+00  4.00000000e+00 -1.00000000e+00]
 [ 0.00000000e+00 -1.00000000e+00 -1.00000000e+00  3.00000000e+00]]

RHS Vector, b:
[ 1.10000000e+01  1.80000000e+01  2.10000000e+01  2.00000000e+01]

Preconditioner Matrix, Minv: <4 x 4, e>:
[[ 2.50000000e-01  0.00000000e+00  0.00000000e+00  0.00000000e+00]
 [ 0.00000000e+00  3.33333333e-01  0.00000000e+00  0.00000000e+00]
 [ 0.00000000e+00  0.00000000e+00  2.50000000e-01  0.00000000e+00]
 [ 0.00000000e+00  0.00000000e+00  0.00000000e+00  3.33333333e-01]]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
PCG Converged: true
PCG Error: 1.2764805630759987e-17
PCG Iterations: 3
Solution Vector, x:
[ 9.09473684e+00  1.40736842e+01  1.13052632e+01  1.51263158e+01]
===============================================================================
```