# Project #05
- Author: Kirill Shumilov
- Date: 04/4/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
MPI Library
```

# File Tree
```
shumilov_project05
.
├── CMakeLists.txt
├── README.md
├── analysis
│   ├── analysis.ipynb
│   ├── analysis.pdf
│   └── utils
│       ├── __init__.py
│       ├── inputs.py
│       ├── params.py
│       ├── special.py
│       └── system.py
├── compile_project05.sh
├── examples
│   ├── s12_gs.inp
│   ├── s12_pj.flux
│   ├── s12_pj.inp
│   ├── s12_sor.inp
│   ├── s4_gs.inp
│   ├── s4_pj.inp
│   ├── s4_sor.inp
│   └── s8_pj.inp
├── include
│   ├── array.h
│   ├── block.h
│   ├── config.h
│   ├── domain.h
│   ├── grid.h
│   ├── header.h
│   ├── inputs.h
│   ├── io.h
│   ├── material.h
│   ├── math.h
│   ├── matrix.h
│   ├── mpi_types.h
│   ├── neighborhood.h
│   ├── point_jacobi.h
│   ├── project.h
│   ├── region.h
│   ├── residual.h
│   ├── result.h
│   ├── shape.h
│   ├── sor.h
│   ├── stencil.h
│   └── utils.h
├── src
│   ├── main.cpp
│   └── mpi_types.cpp
├── submit_project05.n16.sh
├── submit_project05.n4.sh
├── submit_project05.n64.sh
└── submit_project05.sh
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
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release --fresh
```

## 2. Compile
Please, make sure to compile all the code on the login node, **before** running it on the compute node.
### Compile MPI Parallel Code
The main parallel MPI utility can be compiled using:
```bash
cmake --build build --config Release --target project05 -- -j
```

### Compile with single command
```bash
[kshumil@login02 shumilov_project05]$ ./compile_project05.sh
```

At this point the executable can be found in:
```
Usage: shumilov_project03 [--help] [--output VAR] [--flux VAR] input

Positional arguments:
  input         Path to input file. 

Optional arguments:
  -h, --help    shows help message and exits 
  -o, --output  Path to output file. 
  -f, --flux    Path to flux file 
```
The code primarily outputs to stdout. To capture the output to the file use `>` operator:

## Examples
### Running serially for small system
```
================================================================================
NE 501 Project #5
Author: Kirill Shumilov
Date: 04/04/2025
--------------------------------------------------------------------------------
Parallel implementation of PJ, GS, and SOR
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Algorithm: Point Jacobi
Tolerance: 1e-07
Maximum #Iterations: 10000
................................................................................
Space Dimensions, a x b: 1.00000000e+00 x 1.00000000e+00
Non-Zero Grid Points, M x N:     4 x     4
Diffusion Coefficient, D: 1.00000000e+00
Macroscopic Removal Cross Section, Sa: 2.00000000e+00
................................................................................
Source:
[[ 1.000000e+00  2.000000e+00  3.000000e+00  4.000000e+00] 
 [ 5.000000e+00  6.000000e+00  7.000000e+00  8.000000e+00] 
 [ 9.000000e+00  1.000000e+01  1.100000e+01  1.200000e+01] 
 [ 1.300000e+01  1.400000e+01  1.500000e+01  1.600000e+01]]
================================================================================
                                 MPI Parameters                                 
--------------------------------------------------------------------------------
Processes: 2
Topology: Cartesian
Dimensions:     2 x     1
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged: true
#Iterations: 66
Iterative Error: 9.562129559725818e-08
Max Abs Residual: 2.654560878348782e-06
Solution:
[[ 1.310706e-01  2.160610e-01  2.403775e-01  1.826385e-01] 
 [ 2.787070e-01  4.300808e-01  4.620409e-01  3.447875e-01] 
 [ 3.759731e-01  5.579211e-01  5.898811e-01  4.420536e-01] 
 [ 3.373422e-01  4.803830e-01  5.046995e-01  3.889101e-01]]
================================================================================
Execution time: 00.000473000 seconds.
================================================================================
```

### Running MPI Parallel with larger system
#### On Login node (Not Recommended)
```
[kshumil@login02 shumilov_project05]$ mpirun -n 8 ./shumilov_project05 examples/s12_pj.inp
================================================================================
NE 501 Project #5
Author: Kirill Shumilov
Date: 04/04/2025
--------------------------------------------------------------------------------
Parallel implementation of PJ, GS, and SOR
================================================================================
                                     Inputs                                     
--------------------------------------------------------------------------------
Algorithm: Point Jacobi
Tolerance: 1e-08
Maximum #Iterations: 10000
................................................................................
Space Dimensions, a x b: 1.00000000e+00 x 1.00000000e+00
Non-Zero Grid Points, M x N:    12 x    12
Diffusion Coefficient, D: 1.00000000e+00
Macroscopic Removal Cross Section, Sa: 2.00000000e+00
................................................................................
Source:
<12, 12>
================================================================================
                                 MPI Parameters                                 
--------------------------------------------------------------------------------
Processes: 8
Topology: Cartesian
Dimensions:     4 x     2
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Converged: true
#Iterations: 477
Iterative Error: 9.8875283866973e-09
Max Abs Residual: 2.1512842425294877e-05
Solution:
<12, 12>
================================================================================
Execution time: 00.012274000 seconds.
================================================================================
[kshumil@login02 shumilov_project05]$ cat examples/s12_pj.flux
  i     j        Flux     
    1     1  2.12145550e-01
    1     2  3.99074444e-01
    1     3  5.52203419e-01
    1     4  6.69947514e-01
    1     5  7.52690824e-01
    1     6  8.00911377e-01
    1     7  8.14378472e-01
    1     8  7.91771959e-01
    1     9  7.30497017e-01
    1    10  6.26608605e-01
    1    11  4.74812073e-01
    1    12  2.68526289e-01
    2     1  4.52018361e-01
    2     2  8.30754436e-01
    2     3  1.13449237e+00
    2     4  1.36507273e+00
    2     5  1.52514338e+00
    2     6  1.61646868e+00
    2     7  1.63896524e+00
    2     8  1.59016234e+00
    2     9  1.46491519e+00
    2    10  1.25528640e+00
    2    11  9.50560895e-01
    2    12  5.37382158e-01
    3     1  6.99516886e-01
    3     2  1.27034094e+00
    3     3  1.72052464e+00
    3     4  2.05810503e+00
    3     5  2.28971581e+00
    3     6  2.41939289e+00
    3     7  2.44773869e+00
    3     8  2.37138948e+00
    3     9  2.18270812e+00
    3    10  1.86965605e+00
    3    11  1.41583470e+00
    3    12  8.00706339e-01
    4     1  9.41974740e-01
    4     2  1.69767244e+00
    4     3  2.28567537e+00
    4     4  2.72169996e+00
    4     5  3.01763877e+00
    4     6  3.18068272e+00
    4     7  3.21265975e+00
    4     8  3.10958069e+00
    4     9  2.86135358e+00
    4    10  2.45165488e+00
    4    11  1.85798757e+00
    4    12  1.05198374e+00
    5     1  1.16883953e+00
    5     2  2.09385467e+00
    5     3  2.80500184e+00
    5     4  3.32682099e+00
    5     5  3.67748201e+00
    5     6  3.86807723e+00
    5     7  3.90213593e+00
    5     8  3.77528194e+00
    5     9  3.47497786e+00
    5    10  2.98036388e+00
    5    11  2.26227569e+00
    5    12  1.28358407e+00
    6     1  1.36933750e+00
    6     2  2.43874345e+00
    6     3  3.25099370e+00
    6     4  3.84069577e+00
    6     5  4.23321934e+00
    6     6  4.44417496e+00
    6     7  4.47917742e+00
    6     8  4.33366752e+00
    6     9  3.99267518e+00
    6    10  3.43053964e+00
    6    11  2.61074451e+00
    6    12  1.48615480e+00
    7     1  1.53094264e+00
    7     2  2.70870211e+00
    7     3  3.59114321e+00
    7     4  4.22442013e+00
    7     5  4.64192379e+00
    7     6  4.86420440e+00
    7     7  4.89920686e+00
    7     8  4.74237197e+00
    7     9  4.37639953e+00
    7    10  3.77068915e+00
    7    11  2.88070317e+00
    7    12  1.64775994e+00
    8     1  1.63781315e+00
    8     2  2.87408219e+00
    8     3  3.78508591e+00
    8     4  4.43012400e+00
    8     5  4.85108126e+00
    8     6  5.07345538e+00
    8     7  5.10751408e+00
    8     8  4.94888120e+00
    8     9  4.57828087e+00
    8    10  3.96044795e+00
    8    11  3.04250321e+00
    8    12  1.75255769e+00
    9     1  1.66856877e+00
    9     2  2.89578189e+00
    9     3  3.78091247e+00
    9     4  4.39754342e+00
    9     5  4.79552117e+00
    9     6  5.00443556e+00
    9     7  5.03641259e+00
    9     8  4.88746309e+00
    9     9  4.53719704e+00
    9    10  3.94689199e+00
    9    11  3.05609702e+00
    9    12  1.77857777e+00
   10     1  1.59237912e+00
   10     2  2.71986928e+00
   10     3  3.51010161e+00
   10     4  4.04985923e+00
   10     5  4.39406030e+00
   10     6  4.57394416e+00
   10     7  4.60228996e+00
   10     8  4.47573398e+00
   10     9  4.17446233e+00
   10    10  3.65923302e+00
   10    11  2.86536305e+00
   10    12  1.69356857e+00
   11     1  1.36086991e+00
   11     2  2.26843193e+00
   11     3  2.88041758e+00
   11     4  3.28885428e+00
   11     5  3.54619554e+00
   11     6  3.68048138e+00
   11     7  3.70297794e+00
   11     8  3.61121450e+00
   11     9  3.38869673e+00
   11    10  3.00121161e+00
   11    11  2.38823839e+00
   11    12  1.44623371e+00
   12     1  8.88714416e-01
   12     2  1.42344000e+00
   12     3  1.76647685e+00
   12     4  1.99005550e+00
   12     5  2.12962526e+00
   12     6  2.20271897e+00
   12     7  2.21618607e+00
   12     8  2.16870640e+00
   12     9  2.05060500e+00
   12    10  1.84088204e+00
   12    11  1.49917763e+00
   12    12  9.45095155e-01

```
#### On Compute node (Recommended)
```bash
[kshumil@login02 shumilov_project05]$ bsub < submit_project05.sh
```