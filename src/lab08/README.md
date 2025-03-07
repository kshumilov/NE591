# Outlab #08
- Author: Kirill Shumilov
- Date: 03/07/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
MPI Library
```


# Hazel HPC System (NCSU)
The code has been run and tested on Hazel. Before continuing login in on the login node of the cluster:
```bash
ssh -X $USER:login.hpc.ncsu.edu
```
Load the latest GCC compiler, CMake, and MPI Implementation:
```bash
module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64
```

## 1. Initialize the build directory
```bash
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
```
### Example
```
[kshumil@login02 NE591]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test COMPILER_SUPPORTS_MARCH
-- Performing Test COMPILER_SUPPORTS_MARCH - Success
-- Performing Test COMPILER_SUPPORTS_FAST_MATH
-- Performing Test COMPILER_SUPPORTS_FAST_MATH - Success
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/build/_deps/json-src/include/
-- Found MPI_CXX: /usr/lib64/mpich/lib/libmpicxx.so (found version "3.1")
-- Found MPI: TRUE (found version "3.1")
-- Run: /usr/lib64/mpich/bin/mpiexec -n 32  EXECUTABLE  ARGS
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/build
```

## 2. Compile
Please, make sure to compile all the code on the login node, **before** running it on the compute node.
### Compile MPI Parallel Code
The main parallel MPI utility can be compiled using:
```bash
cmake --build build --config Release --target outlab08 -- -j
```

#### Example
```
[kshumil@login02 NE591]$ cmake --build build --target outlab08 -- -j
[ 16%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 33%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 50%] Linking CXX static library libfmt.a
[ 50%] Built target fmt
[ 66%] Building CXX object src/lab08/CMakeFiles/outlab08.dir/src/mpi_utils.cpp.o
[ 83%] Building CXX object src/lab08/CMakeFiles/outlab08.dir/src/outlab08.cpp.o
[100%] Linking CXX executable shumilov_outlab08
[100%] Built target outlab08
```

At this point the executable can be found in:
```
[kshumil@login02 shumilov_outlab08]$ ./shumilov_outlab08 -h
Usage: shumilov_outlab08 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file.

Optional arguments:
  -h, --help    shows help message and exits
  -o, --output  Path to output file
```

### Compile Serial Code
The executable with serial algorithm implementation can be compiled using the following command:
```bash
cmake --build build --config Release --target inlab08 -- -j
```

#### Example
```
[kshumil@login02 NE591]$ cmake --build build --target inlab08 -- -j
Consolidate compiler generated dependencies of target fmt
[ 60%] Built target fmt
[ 80%] Building CXX object src/lab08/CMakeFiles/inlab08.dir/src/inlab08.cpp.o
[100%] Linking CXX executable shumilov_inlab08
[100%] Built target inlab08
```

At this point the executable can be found in:
```
[kshumil@login02 shumilov_outlab08]$ ./shumilov_inlab08 -h
Usage: shumilov_inlab08 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file.

Optional arguments:
  -h, --help    shows help message and exits
  -o, --output  Path to output file
```

### Compile Gauss-Legendre Quadrature Example
To test correctness of the Gauss-Legendre quadrature, you can compile `quad` utility. The program outputs to `stdout`
the nodes and weights of Gauss-Legendre quadrature.

```bash
cmake --build build --target quad -- -j
```

#### Example
```
[kshumil@login02 NE591]$ cmake --build build --target inlab08 -- -j
[ 60%] Built target fmt
[ 80%] Building CXX object src/lab08/CMakeFiles/quad.dir/src/gauss_quad.cpp.o
[100%] Linking CXX executable quad
[100%] Built target quad
```

At this point the executable can be found in:
```
[kshumil@login02 shumilov_outlab08]$ ./gauss_quad -h
Usage: gauss_quad [--help] [--version] [--tol VAR] [--iter VAR] n

Generates Gauss-Legendre quadrature nodes and weights,
derived from Legendre Polynomial of degree n

Positional arguments:
  n              Number of nodes to generate; n > 0

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  --tol          Newton-Raphson absolute tolerance: eps > 0 [nargs=0..1] [default: 1e-12]
  --iter         Maximum number of iterations: N > 0 [nargs=0..1] [default: 10]
```

### Compiling all three utilities
To compile all three utilities, please, execute:
```bash
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
cmake --build build
```

### Getting convergence information printed to stderr
To see more detailed convergence information printed to stderr,
please compile the project in `Debug` build type:
```bash
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
cmake --build build
```

## 3. Timing Information
Simple timing information was gathered. All runs we run with maximum of 100 iterations and final tolerance of $10^{-12}$.
The input file and output files can be found under `examples/a64s20k.inp` and `examples/a64s20.*.out`, respectively:

| P | I      | N  | Time, s   | Error        | Iterations |
|---|--------|----|-----------|--------------|------------|
| 1 | 20_000 | 64 | 00.387268 | 5.065611e-09 | 17         |
| 2 | 20_000 | 64 | 00.200651 | 5.065611e-09 | 17         |
| 4 | 20_000 | 64 | 00.115598 | 5.065611e-09 | 17         |
| 6 | 20_000 | 64 | 00.081034 | 5.065611e-09 | 17         |

## 4. Examples
### Running Serial with output to stdout
```bash
[kshumil@login02 shumilov_outlab08]$ ./shumilov_inlab08 examples/a32s10.inp
================================================================================
NE 591 Inlab #08
Author: Kirill Shumilov
Date: 02/28/2025
--------------------------------------------------------------------------------
Solving Source Iteration Equations
================================================================================
Slab:
Slab Grid:
	Number of angular grid points, N        : 64
	Number of spatial cells, I              : 10
Medium:
	Total Macroscopic Cross Section, St     :   1.000000e+00
	Scattering Macroscopic Cross Section, Ss:   5.000000e-01
	Uniform Fixed Source Strength, q        :   1.000000e+00
	Slab Width, L                           :   1.000000e+00
--------------------------------------------------------------------------------
Fixed-Point Iteration:
	Maximum Number of Iterations............: 100
	Tolerance...............................: 1.000000e-05
--------------------------------------------------------------------------------
SUCCESS at #11 with error   5.691750e-06
--------------------------------------------------------------------------------
  i        flux
    1   7.040376e-01
    2   8.432337e-01
    3   9.141558e-01
    4   9.608904e-01
    5   9.817911e-01
    6   9.817911e-01
    7   9.608904e-01
    8   9.141558e-01
    9   8.432337e-01
   10   7.040376e-01

--------------------------------------------------------------------------------
Execution time: 00.000267000 seconds.
================================================================================
```
