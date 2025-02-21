# Outlab #06
- Author: Kirill Shumilov
- Date: 02/20/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# File Tree
The project `main.cxx` function is located under `src`
```
humilov_outlab06/
├── CMakeLists.txt
├── README.md
├── analysis   <-- Report Location
│   ├── analysis.ipynb
│   ├── analysis.pdf
│   └── timing_info.json
├── examples   <--- Example Input and Output Files
│   ├── gs.inp
│   ├── gs.out
│   ├── pj.inp
│   ├── pj.out
│   ├── sor.inp
│   └── sor.out
├── include
│   ├── build_system.h    <--- Task 4 Builder Function Implementation
│   ├── lab06.h           <--- General lab06 implementation for main.cxx and custom_system.cxx
│   └── methods
│       ├── array.h
│       ├── linalg
│       │   ├── Axb   <--- Root for Iterative Ax=b solvers
│       │   │   ├── algorithm.h
│       │   │   ├── gauss_seidel.h
│       │   │   ├── io.h
│       │   │   ├── point_jacobi.h
│       │   │   ├── solve.h
│       │   │   ├── sor.h
│       │   │   └── utils.h
│       │   ├── blas.h
│       │   ├── lu.h
│       │   ├── matrix.h
│       │   ├── utils
│       │   │   ├── io.h
│       │   │   └── math.h
│       │   └── vec.h
│       ├── optimize.h   <--- Fixed Point Iteration Implementation
│       └── utils
│           ├── io.h
│           └── math.h
└── src
    ├── compare_methods.cxx <--- Executable to compare performance of different methods
    ├── custom_system.cxx   <--- Task 4 Solving the built system
    └── main.cxx            <--- Main Outlab06 executable (in style of Inlab06 and Lab5)

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
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
```
### Example
```bash
[kshumil@login03 shumilov_outlab06]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_outlab06/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_outlab06/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release -- -j
```

### Example
```bash
[kshumil@login03 shumilov_outlab06]$ cmake --build build --config Release -- -j
[ 11%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 22%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 33%] Linking CXX static library libfmt.a
[ 33%] Built target fmt
[ 66%] Building CXX object CMakeFiles/custom_system.dir/src/custom_system.cxx.o
[ 66%] Building CXX object CMakeFiles/compare_methods.dir/src/compare_methods.cxx.o
[ 66%] Building CXX object CMakeFiles/outlab06.dir/src/main.cxx.o
[ 77%] Linking CXX executable custom_system
[ 77%] Built target custom_system
[ 88%] Linking CXX executable compare_methods
[ 88%] Built target compare_methods
[100%] Linking CXX executable shumilov_outlab06
[100%] Built target outlab06
```

At this point three executables can be found in the root directory.

One as a main executable for the Outlab06:
```bash
[kshumil@login03 shumilov_outlab06]$ ./shumilov_outlab06 -h
Usage: shumilov_outlab6 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file

Optional arguments:
  -h, --help    shows help message and exits
  -o, --output  Path to output file
```

Custom matrix builder and solver for Task 4
```bash
[kshumil@login03 shumilov_outlab06]$ ./custom_system -h
Usage: generate_matrix [--help] [--output VAR] [--algorithm VAR] [--tolerance VAR] [--max-iterations VAR] [--relaxation-factor VAR] n

Positional arguments:
  n                         Matrix Rank

Optional arguments:
  -h, --help                shows help message and exits
  -o, --output              Path to output file, if not provided the output is written to stdout
  -a, --algorithm           Algorithm to use [nargs=0..1] [default: "sor"]
  --tol, --tolerance        Convergence tolerance [nargs=0..1] [default: 1e-08]
  --iter, --max-iterations  Maximum number of iterations [nargs=0..1] [default: 10]
  -w, --relaxation-factor   Relaxation factor for SOR (ignored otherwise) [nargs=0..1] [default: 1.05]
```
And method comparator
```bash
Usage: compare_methods [--help] [-s VAR] [-l VAR] [--iterations VAR] [--tolerance VAR] [--relaxation-factor VAR] [--output-json VAR]

Compares performance of LUP, GS, PJ, and SOR methods for solving Ax=b problem

Optional arguments:
  -h, --help               shows help message and exits
  -s                       Smallest power of two to generate rank: n = 2^s [nargs=0..1] [default: 5]
  -l                       Largest power of two to generate rank: n = 2^l [nargs=0..1] [default: 8]
  --iter, --iterations     Maximum number of iterations [nargs=0..1] [default: 100]
  --tol, --tolerance       Maximum tolerance [nargs=0..1] [default: 0.0001]
  -w, --relaxation-factor  Relaxation factor for SOR, ignored otherwise [nargs=0..1] [default: 1.8]
  --output-json            Path to json-formatted timings
```

Only `shumilov_outlab06` requires an input file, which are provided `examples/` directory

# Examples
## Main Executable
For executable to write to file please execute the following, if no `-o` flag is provided the output will be written to `stdout`:
```bash
[kshumil@login02 shumilov_outlab06]$ ./shumilov_outlab06 examples/sor.inp -o examples/sor.out
```

## Custom System
For executable to write to file please execute the following, if no `-o` flag is provided the output will be written to `stdout`:
```bash
[kshumil@login02 shumilov_outlab06]$ ./custom_system 5 --iter 100 --tol 1e-6 -o examples/n5.out
```

## Compare Methods
```bash
.[kshumil@login02 shumilov_outlab06]$ ./compare_methods -s 5 -l 10 --iter 40000 --tol 1e-8 -w 1.9
Testing matrix <32 x 32, e>
LUP Residual 2.403633e-14 in 00.000013000 sec
PJ  Converged at iteration #751   with residual 9.945182e-09 in 00.000458000 sec (LUP Deviation =  4.70138e-09)
GS  Converged at iteration #392   with residual 9.542205e-09 in 00.000280000 sec (LUP Deviation =  2.29348e-09)
SOR Converged at iteration #189   with residual 8.367948e-09 in 00.000161000 sec (LUP Deviation =  1.49877e-10)

Testing matrix <64 x 64, e>
LUP Residual 1.585954e-13 in 00.000040000 sec
PJ  Converged at iteration #1598  with residual 9.901356e-09 in 00.003570000 sec (LUP Deviation =  3.92171e-09)
GS  Converged at iteration #830   with residual 9.873062e-09 in 00.001883000 sec (LUP Deviation =  1.97114e-09)
SOR Converged at iteration #188   with residual 8.552774e-09 in 00.000441000 sec (LUP Deviation =  5.73268e-11)

Testing matrix <128 x 128, e>
LUP Residual 4.077849e-13 in 00.000264000 sec
PJ  Converged at iteration #3322  with residual 9.978123e-09 in 00.034124000 sec (LUP Deviation =  3.37066e-09)
GS  Converged at iteration #1723  with residual 9.908319e-09 in 00.015810000 sec (LUP Deviation =  1.68023e-09)
SOR Converged at iteration #186   with residual 9.872679e-09 in 00.001744000 sec (LUP Deviation =  2.50062e-11)

Testing matrix <256 x 256, e>
LUP Residual 1.198500e-12 in 00.001665000 sec
PJ  Converged at iteration #6813  with residual 9.996256e-09 in 00.314946000 sec (LUP Deviation =  2.92672e-09)
GS  Converged at iteration #3530  with residual 9.946348e-09 in 00.164233000 sec (LUP Deviation =  1.45903e-09)
SOR Converged at iteration #192   with residual 9.668724e-09 in 00.008928000 sec (LUP Deviation =  9.48686e-12)

Testing matrix <512 x 512, e>
LUP Residual 3.974210e-12 in 00.016617000 sec
PJ  Converged at iteration #13857 with residual 9.996256e-09 in 03.176058000 sec (LUP Deviation =  2.57184e-09)
GS  Converged at iteration #7174  with residual 9.985570e-09 in 01.646325000 sec (LUP Deviation =  1.28667e-09)
SOR Converged at iteration #342   with residual 9.480573e-09 in 00.078550000 sec (LUP Deviation =  1.00023e-10)

Testing matrix <1024 x 1024, e>
LUP Residual 1.571582e-11 in 00.111407000 sec
PJ  Converged at iteration #28039 with residual 9.988980e-09 in 30.704779000 sec (LUP Deviation =  2.28981e-09)
GS  Converged at iteration #14511 with residual 9.984433e-09 in 16.003810000 sec (LUP Deviation =  1.14475e-09)
SOR Converged at iteration #781   with residual 9.816176e-09 in 00.849284000 sec (LUP Deviation =  1.03735e-10)
```
