# Outlab #04
- Author: Kirill Shumilov
- Date: 02/07/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.26
```

# File Tree
The project `main.cxx` function is located under `src`
```
shumilov_outlab04  <-- Root Directory (Run commands from this directory)
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
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target outlab04 -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab04]$ cmake --build build --config Release --target outlab04 -- -j
[ 50%] Building CXX object CMakeFiles/outlab04.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_outlab04
[100%] Built target outlab04
```

At this point the executable can be found in:
```bash
[kshumil@login02 shumilov_outlab04]$ ./shumilov_outlab04 -h
Usage: shumilov_outlab04 [--help] [--version] filename

This program solves system of equations Ax=LUx=b
by performing LU factorization of A and then
using forward and backward substitution based on
lower triangular (L), upper-triangular (U), and RHS vector (b)

Positional arguments:
  filename       Path to input fle 

Optional arguments:
  -h, --help     shows help message and exits 
  -v, --version  prints version information and exits 
```

# Examples
```bash
```
