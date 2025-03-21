# Inlab #09
- Author: Kirill Shumilov
- Date: 03/21/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
```
shumilov_inlab09
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
    └── inlab09.cpp
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
[kshumil@login02 shumilov_inlab09]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_inlab09/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_inlab09/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build -- -j
```

### Example
```bash
[kshumil@login02 shumilov_inlab09]$ cmake --build build -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/inlab09.dir/src/inlab09.cpp.o
[100%] Linking CXX executable shumilov_inlab09
[100%] Built target inlab09
```

At this point the executable can be found in:
```bash
[kshumil@login02 shumilov_inlab09]$ ./shumilov_inlab09 -h
Usage: shumilov_inlab09 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file.

Optional arguments:
  -h, --help    shows help message and exits
  -o, --output  Path to output file
```

# Examples
## Printing to stdout
```bash
[kshumil@login02 shumilov_inlab09]$ ./shumilov_inlab09 examples/symm.inp
================================================================================
NE 501 Inlab #09
Author: Kirill Shumilov
Date: 03/21/2025
--------------------------------------------------------------------------------
Preparation for implementation of CG solver for Ax=b systems
================================================================================
                                     Inputs
Fixed-Point Iteration:
	Maximum Number of Iterations............: 10
	Tolerance...............................: 1.000000e-05
Matrix, A...............................: <5 x 5, e>
[[ 1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00]
 [ 2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00]
 [ 3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00]
 [ 4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00]
 [ 5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00]]
RHS Vector, b...........................: <5, e>
[1.1, 1.2, 1.3, 1.4, 1.5]
--------------------------------------------------------------------------------

```

## Writing into a txt file
```bash
[kshumil@login02 shumilov_inlab09]$ ./shumilov_inlab09 examples/symm.inp --output out
[kshumil@login02 shumilov_inlab09]$ cat out
================================================================================
NE 501 Inlab #09
Author: Kirill Shumilov
Date: 03/21/2025
--------------------------------------------------------------------------------
Preparation for implementation of CG solver for Ax=b systems
================================================================================
                                     Inputs
Fixed-Point Iteration:
	Maximum Number of Iterations............: 10
	Tolerance...............................: 1.000000e-05
Matrix, A...............................: <5 x 5, e>
[[ 1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00]
 [ 2.000000e+00  3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00]
 [ 3.000000e+00  4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00]
 [ 4.000000e+00  5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00]
 [ 5.000000e+00  1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00]]
RHS Vector, b...........................: <5, e>
[1.1, 1.2, 1.3, 1.4, 1.5]
--------------------------------------------------------------------------------
```