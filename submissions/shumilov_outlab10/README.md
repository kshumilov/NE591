# Inlab #09
- Author: Kirill Shumilov
- Date: 03/27/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
```
shumilov_outlab10
├── CMakeLists.txt
├── README.md
├── examples
│   ├── asymm.inp
│   └── symm.inp
├── include
│   ├── CMakeLists.txt
│   ├── lab
│   │   ├── config.h
│   │   └── lab.h
│   └── methods
│       ├── linalg
│       │   ├── blas.h
│       │   ├── matrix.h
│       │   ├── utils
│       │   │   ├── io.h
│       │   │   └── math.h
│       │   └── vec.h
│       ├── optimize.h
│       └── utils
│           ├── io.h
│           └── math.h
├── lab_include
│   ├── inputs.h
│   └── util.h
└── src
    └── outlab10.cpp
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
[kshumil@login02 shumilov_outlab10]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_outlab10/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_outlab10/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab10]$ cmake --build build -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/outlab10.dir/src/outlab10.cpp.o
[100%] Linking CXX executable shumilov_outlab10
[100%] Built target outlab10
```

At this point the executable can be found in:
```bash
sage: shumilov_outlab10 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file. 

Optional arguments:
  -h, --help    shows help message and exits 
  -o, --output  Path to output file 
```

# Examples
## Printing to stdout
```bash
================================================================================
NE 501 Outlab #10
Author: Kirill Shumilov
Date: 03/27/2025
--------------------------------------------------------------------------------
Implementation of CG solver for Ax=b systems
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Tolerance: 1e-05
Maximum #Iterations: 10
Matrix, A: <5 x 5, e>:
[[ 1.00000000e+00  2.00000000e+00  3.00000000e+00  4.00000000e+00  5.00000000e+00]
 [ 2.00000000e+00  3.00000000e+00  4.00000000e+00  5.00000000e+00  1.00000000e+00]
 [ 3.00000000e+00  4.00000000e+00  5.00000000e+00  1.00000000e+00  2.00000000e+00]
 [ 4.00000000e+00  5.00000000e+00  1.00000000e+00  2.00000000e+00  3.00000000e+00]
 [ 5.00000000e+00  1.00000000e+00  2.00000000e+00  3.00000000e+00  4.00000000e+00]]

RHS Vector, b:
[ 1.10000000e+00  1.20000000e+00  1.30000000e+00  1.40000000e+00  1.50000000e+00]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
CG Converged: true
CG Error: 9.379305353149599e-14
CG Iterations: 5
Solution Vector, x:
[ 1.66666667e-01  6.66666667e-02  6.66666667e-02  6.66666667e-02  6.66666667e-02]
================================================================================
```

## Writing into a txt file
```bash
================================================================================
NE 501 Outlab #10
Author: Kirill Shumilov
Date: 03/27/2025
--------------------------------------------------------------------------------
Implementation of CG solver for Ax=b systems
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Tolerance: 1e-05
Maximum #Iterations: 10
Matrix, A: <5 x 5, e>:
[[ 1.00000000e+00  2.00000000e+00  3.00000000e+00  4.00000000e+00  5.00000000e+00]
 [ 2.00000000e+00  3.00000000e+00  4.00000000e+00  5.00000000e+00  1.00000000e+00]
 [ 3.00000000e+00  4.00000000e+00  5.00000000e+00  1.00000000e+00  2.00000000e+00]
 [ 4.00000000e+00  5.00000000e+00  1.00000000e+00  2.00000000e+00  3.00000000e+00]
 [ 5.00000000e+00  1.00000000e+00  2.00000000e+00  3.00000000e+00  4.00000000e+00]]

RHS Vector, b:
[ 1.10000000e+00  1.20000000e+00  1.30000000e+00  1.40000000e+00  1.50000000e+00]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
CG Converged: true
CG Error: 9.379305353149599e-14
CG Iterations: 5
Solution Vector, x:
[ 1.66666667e-01  6.66666667e-02  6.66666667e-02  6.66666667e-02  6.66666667e-02]
================================================================================
```