# Project #02
- Author: Kirill Shumilov
- Date: 02/28/2025

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
├── analysis
│   ├── Analysis.ipynb
│   └── Analysis.pdf
├── examples
│   ├── data.csv
│   ├── l64.json
│   ├── l64_result.json
│         ....
├── include
│   ├── methods
│   │   ├── array.h
│   │   ├── linalg
│   │   │   ├── blas.h  <-- implementation of BLAS Operations
│   │   │   ├── lu.h    <-- implementation of LU Factorization
│   │   │   ├── matrix.h  <-- Matrix abstructions
│   │   │   ├── utils
│   │   │   │   └── io.h
│   │   │   └── vec.h    <--- Vector abstructions
│   │   └── utils
│   │       ├── io.h
│   │       └── math.h
│   └── project
│       ├── diffusion_problem.h   <-- diffusion problem setup
│       └── diffusion_solver.h   <-- diffusion solver setup
├── shumilov_project01
└── src
    └── main.cxx   <-- Entry point

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
[kshumil@login03 shumilov_projec02]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_projec02/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_projec02/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target project02 -- -j
```

### Example
```bash
[kshumil@login03 shumilov_projec02]$ cmake --build build --config Release -- -j
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/project01.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_project02
[100%] Built target project02
```

At this point the executable can be found in:
```bash
[kshumil@login03 shumilov_projec02]$ ./shumilov_project02 -h
Usage: shumilov_projec02 [--help] [--output VAR] [--output-json] [--input-json] input

Positional arguments:
  input          Path to input file. 

Optional arguments:
  -h, --help     shows help message and exits 
  -o, --output   Path to output file 
  --output-json  Write the output file in json-format 
  --input-json   Path to parameter files 
```

# Examples
## Successful LUP Factorization and Solution
```bash
[kshumil@login02 shumilov_outlab05]$ cat examples/s5_sor.inp
3

100 1e-7 1.5

1.0 1.0

7 7

1.0 2.0

0 0 0 0 0 0 0
0 0 0 0 0 0 0
0 0 .5 .5 .5 0 0
0 0 .5 1 .5 0 0
0 0 .5 .5 .5 0 0
0 0 0 0 0 0 0
0 0 0 0 0 0 0
[kshumil@login03 shumilov_projec02]$ ./shumilov_project02 examples/s5_sor.inp
================================================================================
NE 591 Project #01
Author: Kirill Shumilov
Date: 02/28/2025
--------------------------------------------------------------------------------
Solving 2D steady state, one speed diffusion equation in a non-multiplying,
isotropic scattering homogeneous medium, using LUP, PJ, GS, or SOR
================================================================================
                                     Inputs                                     
Space Dimensions, a x b.................: 1.000000e+00 x 1.000000e+00
Non-Zero Grid Points, M x N.............: 7 x 7
Diffusion Coefficient, D................: 1
Macroscopic Removal Cross Section, Sa...: 2
Source, q...............................:
[[ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  5.000000e-01  5.000000e-01  5.000000e-01  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  5.000000e-01  1.000000e+00  5.000000e-01  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  5.000000e-01  5.000000e-01  5.000000e-01  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]]
................................................................................
Selected Method: Successive Over Relaxation
Fixed-Point Iteration:
	Maximum Number of Iterations: 100
	Tolerance: 1.000000e-07
	Relaxation Factor: 1.500000e+00
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Flux, phi <7 x 7, e>:
[[ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03] 
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03] 
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03] 
 [ 3.703100e-03  8.326864e-03  1.522206e-02  1.898003e-02  1.522206e-02  8.326864e-03  3.703100e-03] 
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03] 
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03] 
 [ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03]]
................................................................................
Max abs residual: 1.491058e-08
Converged at iteration #29   :
	Relative error: 4.049411e-08
................................................................................
Execution time: 00.000109000 seconds.
================================================================================
```

## Writing into a txt file
```bash
[kshumil@login02 shumilov_outlab05]$ ./shumilov_outlab05 examples/s5.inp -o examples/s5_sor.out
================================================================================
NE 591 Project #01
Author: Kirill Shumilov
Date: 02/28/2025
--------------------------------------------------------------------------------
Solving 2D steady state, one speed diffusion equation in a non-multiplying,
isotropic scattering homogeneous medium, using LUP, PJ, GS, or SOR
================================================================================
                                     Inputs                                     
Space Dimensions, a x b.................: 1.000000e+00 x 1.000000e+00
Non-Zero Grid Points, M x N.............: 7 x 7
Diffusion Coefficient, D................: 1
Macroscopic Removal Cross Section, Sa...: 2
Source, q...............................:
[[ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  5.000000e-01  5.000000e-01  5.000000e-01  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  5.000000e-01  1.000000e+00  5.000000e-01  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  5.000000e-01  5.000000e-01  5.000000e-01  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00] 
 [ 0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00  0.000000e+00]]
................................................................................
Selected Method: Successive Over Relaxation
Fixed-Point Iteration:
	Maximum Number of Iterations: 100
	Tolerance: 1.000000e-07
	Relaxation Factor: 1.500000e+00
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Flux, phi <7 x 7, e>:
[[ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03] 
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03] 
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03] 
 [ 3.703100e-03  8.326864e-03  1.522206e-02  1.898003e-02  1.522206e-02  8.326864e-03  3.703100e-03] 
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03] 
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03] 
 [ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03]]
................................................................................
Max abs residual: 1.491058e-08
Converged at iteration #29   :
	Relative error: 4.049411e-08
................................................................................
Execution time: 00.000109000 seconds.
================================================================================
```

## Writing into a json file in quiet mode
```bash
[kshumil@login03 shumilov_projec01]$ ./shumilov_project01 examples/s5_sor.inp -o s5.json --output-json
```
