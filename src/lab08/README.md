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
[kshumil@login03 shumilov_outlab08]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test COMPILER_SUPPORTS_MARCH
-- Performing Test COMPILER_SUPPORTS_MARCH - Success
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
### Compile MPI Parallel Code
Now, compile the code:
```bash
cmake --build build --config Release --target outlab08 -- -j
```

### Example
```bash
[kshumil@login03 shumilov_inlab08]$ cmake --build build --config Release -- -j
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/project01.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_inlab08
[100%] Built target project02
```

At this point the executable can be found in:
```bash
[kshumil@login03 shumilov_inlab08]$ ./shumilov_inlab08 -h
Usage: shumilov_inlab08 [--help] [--output VAR] input

Positional arguments:
  input         Path to input file. 

Optional arguments:
  -h, --help    shows help message and exits 
  -o, --output  Path to output file 
```

# Examples
## Printing to stdout
```bash
[kshumil@login02 shumilov_outlab05]$ cat examples/test.inp
1 10
1.0 0.5
1.0 1.0
1e-5 100
[kshumil@login03 shumilov_inlab08]$ ./shumilov_inlab08 examples/s5_sor.inp
================================================================================
NE 591 Inlab #08
Author: Kirill Shumilov
Date: 02/28/2025
--------------------------------------------------------------------------------
Solving Source Iteration Equations
================================================================================
Slab:
Slab Grid:
	Number of angular grid points, N        : 2
	Number of spatial cells, I              : 10
Medium:
	Total Macroscopic Cross Section, Sa     :   1.000000e+00
	Scattering Macroscopic Cross Section, St:   5.000000e-01
	Uniform Fixed Source Strength, q        :   1.000000e+00
	Slab Width, L                           :   1.000000e+00
--------------------------------------------------------------------------------
Fixed-Point Iteration:
	Maximum Number of Iterations............: 100
	Tolerance...............................: 1.000000e-05
--------------------------------------------------------------------------------
Converged at iteration #18: 9.528349e-06
--------------------------------------------------------------------------------
  i        flux     
    1   1.867031e+00
    2   2.107028e+00
    3   2.287026e+00
    4   2.407025e+00
    5   2.467024e+00
    6   2.467024e+00
    7   2.407025e+00
    8   2.287026e+00
    9   2.107028e+00
   10   1.867031e+00

--------------------------------------------------------------------------------
Execution time: 00.000005000 seconds.
================================================================================

```

## Writing into a txt file
```bash
[kshumil@login02 shumilov_outlab05]$ ./shumilov_inlab08 examples/test.inp -o examples/test.out
================================================================================
NE 591 Inlab #08
Author: Kirill Shumilov
Date: 02/28/2025
--------------------------------------------------------------------------------
Solving Source Iteration Equations
================================================================================
Slab:
Slab Grid:
	Number of angular grid points, N        : 2
	Number of spatial cells, I              : 10
Medium:
	Total Macroscopic Cross Section, Sa     :   1.000000e+00
	Scattering Macroscopic Cross Section, St:   5.000000e-01
	Uniform Fixed Source Strength, q        :   1.000000e+00
	Slab Width, L                           :   1.000000e+00
--------------------------------------------------------------------------------
Fixed-Point Iteration:
	Maximum Number of Iterations............: 100
	Tolerance...............................: 1.000000e-05
--------------------------------------------------------------------------------
Converged at iteration #18: 9.528349e-06
--------------------------------------------------------------------------------
  i        flux     
    1   1.867031e+00
    2   2.107028e+00
    3   2.287026e+00
    4   2.407025e+00
    5   2.467024e+00
    6   2.467024e+00
    7   2.407025e+00
    8   2.287026e+00
    9   2.107028e+00
   10   1.867031e+00

--------------------------------------------------------------------------------
Execution time: 00.000005000 seconds.
================================================================================
```

## Getting convergence information printed to stderr
To see more detailed convergence information printed to stderr,
please compile the project in `Debug` build type:
```bash
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
cmake --build build
```