# Outlab #05
- Author: Kirill Shumilov
- Date: 02/07/2025

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
[kshumil@login03 shumilov_projec01]$ cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type: Release
-- Using the multi-header code from /home/kshumil/ne591/NE591/submissions/shumilov_projec01/build/_deps/json-src/include/
-- Configuring done
-- Generating done
-- Build files have been written to: /home/kshumil/ne591/NE591/submissions/shumilov_projec01/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target project01 -- -j
```

### Example
```bash
[kshumil@login03 shumilov_projec01]$ cmake --build build --config Release -- -j
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object CMakeFiles/project01.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_project01
[100%] Built target project01
```

At this point the executable can be found in:
```bash
[kshumil@login03 shumilov_projec01]$ ./shumilov_project01 -h
Usage: shumilov_project01 [--help] [--input-json] [--output VAR] [--output-json] [--quiet] filename

Solving 2D steady state, one speed diffusion equation in a non-multiplying,
isotropic scattering homogeneous medium, using LUP factorization

Positional arguments:
  filename       Path to input file

Optional arguments:
  -h, --help     shows help message and exits
  --input-json   Read the input file in json-format
  -o, --output   Path to output file
  --output-json  Write the output file in json-format
  --quiet        If present suppresses output to stdout
```

# Examples
## Successful LUP Factorization and Solution
```bash
[kshumil@login02 shumilov_outlab05]$ cat examples/s5.inp
1

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
[kshumil@login03 shumilov_projec01]$ ./shumilov_project01 examples/s5.inp
================================================================================
NE 591 Project #01
Author: Kirill Shumilov
Date: 02/14/2025
--------------------------------------------------------------------------------
Solving 2D steady state, one speed diffusion equation in a non-multiplying,
isotropic scattering homogeneous medium, using LUP factorization
================================================================================
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
================================================================================
                                    Results
--------------------------------------------------------------------------------
Flux, phi<7 x 7, d>:
[[ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03]
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03]
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03]
 [ 3.703100e-03  8.326864e-03  1.522206e-02  1.898003e-02  1.522206e-02  8.326864e-03  3.703100e-03]
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03]
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03]
 [ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03]]
Max Abs Residual: 8.881784e-16
================================================================================
```

## Writing into a txt file
```bash
[kshumil@login02 shumilov_outlab05]$ ./shumilov_outlab05 examples/s5.inp -o s5.out
================================================================================
NE 591 Project #01
Author: Kirill Shumilov
Date: 02/14/2025
--------------------------------------------------------------------------------
Solving 2D steady state, one speed diffusion equation in a non-multiplying,
isotropic scattering homogeneous medium, using LUP factorization
================================================================================
[kshumil@login02 shumilov_outlab05]$ cat ./s5.out
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
================================================================================
                                    Results
--------------------------------------------------------------------------------
Flux, phi<7 x 7, d>:
[[ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03]
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03]
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03]
 [ 3.703100e-03  8.326864e-03  1.522206e-02  1.898003e-02  1.522206e-02  8.326864e-03  3.703100e-03]
 [ 3.300628e-03  7.321257e-03  1.312226e-02  1.522206e-02  1.312226e-02  7.321257e-03  3.300628e-03]
 [ 2.281302e-03  4.764060e-03  7.321257e-03  8.326864e-03  7.321257e-03  4.764060e-03  2.281302e-03]
 [ 1.131809e-03  2.281302e-03  3.300628e-03  3.703100e-03  3.300628e-03  2.281302e-03  1.131809e-03]]
Max Abs Residual: 8.881784e-16
================================================================================
```

## Writing into a json file in quiet mode
```bash
[kshumil@login03 shumilov_projec01]$ ./shumilov_project01 examples/s5.txt -o s5.out --quiet --output-json
```

```bash
[kshumil@login03 shumilov_projec01]$ cat s5.out
```
```json
{
    "flux": {
        "m_cols": 7,
        "m_data": [
            0.0011318085475407573,
            0.0022813016036368383,
            0.003300628449671247,
            0.0037030998138947026,
            0.0033006284496712467,
            0.002281301603636838,
            0.0011318085475407569,
            0.0022813016036368388,
            0.004764060092448999,
            0.007321257020205674,
            0.008326864225420526,
            0.007321257020205673,
            0.004764060092448999,
            0.002281301603636838,
            0.0033006284496712476,
            0.007321257020205674,
            0.013122264595163349,
            0.015222057554420446,
            0.013122264595163345,
            0.007321257020205674,
            0.0033006284496712467,
            0.0037030998138947043,
            0.008326864225420531,
            0.015222057554420453,
            0.01898002610051021,
            0.015222057554420448,
            0.008326864225420524,
            0.003703099813894702,
            0.0033006284496712485,
            0.007321257020205676,
            0.013122264595163349,
            0.015222057554420448,
            0.013122264595163345,
            0.00732125702020567,
            0.003300628449671246,
            0.0022813016036368388,
            0.0047640600924489995,
            0.007321257020205674,
            0.008326864225420526,
            0.00732125702020567,
            0.004764060092448995,
            0.0022813016036368366,
            0.0011318085475407577,
            0.002281301603636839,
            0.003300628449671248,
            0.0037030998138947026,
            0.0033006284496712454,
            0.002281301603636836,
            0.0011318085475407564
        ],
        "m_rows": 7
    },
    "problem": {
        "absorption_scattering": 2.0,
        "diffusion_coefficient": 1.0,
        "grid": {
            "points": {
                "NX": 7,
                "NY": 7
            },
            "space": {
                "X": 1.0,
                "Y": 1.0
            }
        },
        "source": {
            "m_cols": 7,
            "m_data": [
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.5,
                0.5,
                0.5,
                0.0,
                0.0,
                0.0,
                0.0,
                0.5,
                1.0,
                0.5,
                0.0,
                0.0,
                0.0,
                0.0,
                0.5,
                0.5,
                0.5,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0,
                0.0
            ],
            "m_rows": 7
        }
    },
    "residual": [
        -2.7755575615628914e-17,
        -4.8626047202574686e-17,
        1.4673114700815285e-17,
        0.0,
        5.551115123125783e-17,
        -1.47412535764061e-17,
        2.7755575615628914e-17,
        -1.005678974654821e-16,
        -9.560754367878763e-18,
        -2.0514059300603818e-16,
        -4.4009704481240893e-17,
        1.3266656237103154e-16,
        1.3036360358483344e-16,
        -6.047983399258511e-17,
        -1.4829022278554322e-17,
        1.6540497657847969e-16,
        2.220446049250313e-16,
        -7.771561172376096e-16,
        4.440892098500626e-16,
        -3.885780586188048e-16,
        8.326672684688674e-17,
        -1.3183898417423734e-16,
        1.8041124150158794e-16,
        -5.551115123125783e-16,
        6.661338147750939e-16,
        -8.881784197001252e-16,
        4.440892098500626e-16,
        -1.3877787807814457e-16,
        -2.7755575615628914e-17,
        -3.0531133177191805e-16,
        8.881784197001252e-16,
        -4.440892098500626e-16,
        2.220446049250313e-16,
        1.1102230246251565e-16,
        1.1102230246251565e-16,
        0.0,
        0.0,
        -1.1102230246251565e-16,
        2.220446049250313e-16,
        -1.1102230246251565e-16,
        0.0,
        -1.6653345369377348e-16,
        -2.7755575615628914e-17,
        -8.326672684688674e-17,
        -1.1102230246251565e-16,
        0.0,
        1.1102230246251565e-16,
        -5.551115123125783e-17,
        0.0
    ]
}
```
