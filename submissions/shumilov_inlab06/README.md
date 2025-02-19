# Inlab #06
- Author: Kirill Shumilov
- Date: 02/14/2025

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
├── examples
│   ├── test1.txt
│   └── test2.inp
├── include
│   ├── CMakeLists.txt
│   ├── methods
│   │   ├── array.h
│   │   ├── linalg
│   │   │   ├── blas.h
│   │   │   ├── lu.h
│   │   │   ├── matrix.h
│   │   │   ├── pj.h   <--- Point Jacobi Iterations
│   │   │   ├── utils
│   │   │   │   ├── io.h
│   │   │   │   └── math.h
│   │   │   └── vec.h
│   │   ├── optimize.h   <--- Fixed Point Iterations
│   │   ├── roots.h
│   │   └── utils
│   │       ├── io.h
│   │       └── math.h
└── src
    └── main.cxx
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
[kshumil@login02 shumilov_inlab06]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 14.2.0
-- Checking whether CXX compiler has -isysroot
-- Checking whether CXX compiler has -isysroot - yes
-- Checking whether CXX compiler supports OSX deployment target flag
-- Checking whether CXX compiler supports OSX deployment target flag - yes
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /opt/homebrew/bin/g++-14 - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: 
-- Using the multi-header code from /Users/kirill/Documents/NC State/Classes/2025 Spring/shumilov_ne591/submissions/shumilov_inlab06/build/_deps/json-src/include/
-- Configuring done (8.6s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/kirill/Documents/NC State/Classes/2025 Spring/shumilov_ne591/submissions/shumilov_inlab06/build

```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target inlab06 -- -j
```

### Example
```bash
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/inlab06.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_inlab06
[100%] Built target inlab06

```

At this point the executable can be found in:
```bash
================================================================================
NE 591 Inlab #06
Author: Kirill Shumilov
Date: 02/14/2025
--------------------------------------------------------------------------------
Solving Ax=b using iterative methods: PJ, GS, and SOR
================================================================================
Usage: shumilov_project01 [--help] filename

Positional arguments:
  filename    Path to input file 

Optional arguments:
  -h, --help  shows help message and exits 

```

# Examples
## Successful LUP Factorization and Solution
```bash
[kshumil@login02 shumilov_inlab06]$ cat examples/test2.inp
0

3
1e-5 100

 2 -1 -2
-4  6  3
-4 -2  8

-6 17 16
[kshumil@login02 shumilov_inlab06]$ ./shumilov_inlab06 examples/test2.inp
================================================================================
NE 591 Inlab #06
Author: Kirill Shumilov
Date: 02/14/2025
--------------------------------------------------------------------------------
Solving Ax=b using iterative methods: PJ, GS, and SOR
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Selected method: Point-Jacobi
Tolerance: 1e-05
Max Iterations: 100
--------------------------------------------------------------------------------
Original matrix, A:
[[ 2.000000e+00 -1.000000e+00 -2.000000e+00] 
 [-4.000000e+00  6.000000e+00  3.000000e+00] 
 [-4.000000e+00 -2.000000e+00  8.000000e+00]]

RHS vector, b:
[-6.000000e+00  1.700000e+01  1.600000e+01]
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged at iteration #77: 3.905695e-05
Solution Vector, b:
[ 9.999437e-01  1.999979e+00  2.999962e+00]
================================================================================
```