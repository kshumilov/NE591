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
shumilov_outlab11
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
│   │   │   ├── blas.h
│   │   │   ├── eig
│   │   │   │   └── power_iter.h
│   │   │   ├── matrix.h
│   │   │   ├── utils
│   │   │   │   ├── io.h
│   │   │   │   └── math.h
│   │   │   └── vec.h
│   │   └── math.h
│   └── utils
│       └── io.h
├── lab_include
│   └── lab11.h
└── src
    └── outlab11.cpp

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
[kshumil@login03 shumilov_outlab11]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_outlab11/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_outlab11/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab11]$ cmake --build build -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/outlab11.dir/src/outlab11.cpp.o
[100%] Linking CXX executable shumilov_outlab11
[100%] Built target outlab11
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
### Printing to stdout
```bash
[kshumil@login03 shumilov_outlab11]$ ./shumilov_outlab11 examples/symm2.inp
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
[kshumil@login03 shumilov_outlab11]$ ./shumilov_outlab11 examples/symm2.inp --output out
[kshumil@login03 shumilov_outlab11]$ cat out
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
Execution time: 00.000009811 seconds.
================================================================================
```