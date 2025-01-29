# Inlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
```requirements
- C++ Compiler: GCC/Clang supporting C++23 standard
- CMake: >=3.26
```

# File Tree
```
shumilov_inlab02   <-- Root Directory (Run commands from this directory)
├── CMakeLists.txt  <-- Compilation Script (do not edit)
├── README.md       <-- Instructions
├── external        <-- Location of user-defined function
│   ├── CMakeLists.txt  <-- Compilation Script for user-defined function as shared library (do not edit)
│   ├── user_func.cpp   <-- Definition of the user-defined function (EDIT HERE)
│   └── user_func.h     <-- Declaration of user-defined function (do not edit)
├── include
│   ├── array.h         <-- Utility functions for working with arrays
│   └── interpolate.h   <-- Lagrange Interpolation Polynomial Implementation
└── src
    └── main.cxx        <-- Code for the executable
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

Further compilation is done on the login node. If you wish to run the code on a compute node, make sure
you have compiled the code on a login node first.

# Building
To build `inlab02` run the following commands from the root of the project `shumilov_inlab02`.

## 1. Initialize the build directory
```bash
cmake -S. -Bbuild
```
### Example
```bash
[kshumil@login02 shumilov_inlab02]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (5.2s)
-- Generating done (0.1s)
-- Build files have been written to: /home/kshumil/shumilov_inlab02/build
```

## 2. Compile the code
```bash
cmake --build build --config Release --target inlab02 -- -j
```
### Example
```bash
[kshumil@login02 shumilov_inlab02]$ cmake --build build --config Release --target inlab02 -- -j
[ 25%] Building CXX object external/CMakeFiles/user_func.dir/user_func.cpp.o
[ 50%] Linking CXX shared library libuser_func.so
[ 50%] Built target user_func
[ 75%] Building CXX object CMakeFiles/inlab02.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_inlab02
[100%] Built target inlab02
```

At this point the executable can be found in project root directory, `shumilov_inlab02`
```bash
[kshumil@login02 shumilov_inlab02]$ ./shumilov_inlab02 -h
Usage: shumilov_inlab02 [--help] [-n VAR] --samples VAR --points VAR...... [--values VAR...]... [--user-func]

================================================================================
NE 591 Inlab #02: Lagrange Interpolation I/O
Author: Kirill Shumilov
Date: 01/17/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function


Optional arguments:
  -h, --help     shows help message and exits
  -n             Number of interpolation points
  -m, --samples  Number of samples to interpolate the function at [required]
  -x, --points   Distinct real interpolation points in increasing order: {x_i} [nargs: 1 or more] [required] [may be repeated]
  -y, --values   Function values at interpolation points, y_i = f(x_i).
                 Ignored when `--user-func` is provided [nargs: 1 or more] [may be repeated]
  --user-func    Toggle the use of user-defined function in user_func.h
```

## User Supplied function
If the user wishes to supply their own custom function, they should edit `external/user_func.cpp` header file.
```c++
/**
 * \brief User defined function
 *
 * As an example, this function defines sine modulated gaussian.
 * Traditionl std::log, std::exp, std::cos, etc, are availbe in the header <cmath>
 *
 * see https://cplusplus.com/reference/cmath/
 *
 * @param x real value
 */
inline auto user_func(const double x) -> double {
    return std::sin(x) * std::exp(-x * x) * 100.0;
}
```
The library, containing user-defined function must be recompiled after `user_func.cpp` has been edited:
```bash
cmake --build build --config Release --target user_func -- -j
```

### Example
```bash
[kshumil@login02 shumilov_inlab02]$ vim external/user_func.cpp
[kshumil@login02 shumilov_inlab02]$ cmake --build build --config Release --target user_func -- -j
[ 50%] Building CXX object external/CMakeFiles/user_func.dir/user_func.cpp.o
[100%] Linking CXX shared library libuser_func.so
[100%] Built target user_func
```

# Running
Please, find bellow the usage instructions:
```bash
[kshumil@login03 shumilov_inlab02]$ ./shumilov_inlab02 -h
Usage: shumilov_inlab02 [--help] [-n VAR] --samples VAR --points VAR...... [--values VAR...]... [--user-func]

================================================================================
NE 591 Inlab #02: Lagrange Interpolation I/O
Author: Kirill Shumilov
Date: 01/17/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function


Optional arguments:
  -h, --help     shows help message and exits
  -n             Number of interpolation points
  -m, --samples  Number of samples to interpolate the function at [required]
  -x, --points   Distinct real interpolation points in increasing order: {x_i} [nargs: 1 or more] [required] [may be repeated]
  -y, --values   Function values at interpolation points, y_i = f(x_i).
                 Ignored when `--user-func` is provided [nargs: 1 or more] [may be repeated]
  --user-func    Toggle the use of user-defined function in user_func.h
```

WARNING: This function accepts only key-word arguments. Please, see `shumilov_outlab02`,
for program that accepts input from `stdin`.

## Example run
### No Func Definition
```bash
[kshumil@login02 shumilov_inlab02]$ ./shumilov_inlab02 -m 10 -x 1 2 3 4 -y 1 2 3 4
================================================================================
NE 591 Inlab #02: Lagrange Interpolation I/O
Author: Kirill Shumilov
Date: 01/17/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
#samples: m = 10
#points : n = 4
user-defined function: false
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
 i                    x                                    f(x)
--------------------------------------------------------------------------------
   1                    1.000000000000E+00          1.000000000000E+00
   2                    2.000000000000E+00          2.000000000000E+00
   3                    3.000000000000E+00          3.000000000000E+00
   4                    4.000000000000E+00          4.000000000000E+00
--------------------------------------------------------------------------------
Where
i    : index of the interpolated point
x    : position of the interpolated point
f(x) : either user-supplied y-values or values from y = f(x) from the function
================================================================================
                                    Results
--------------------------------------------------------------------------------
 i           x                L(x)
--------------------------------------------------------------------------------
1     1.00000000000e+00  1.00000000000e+00
2     1.33333333333e+00  1.33333333333e+00
3     1.66666666667e+00  1.66666666667e+00
4     2.00000000000e+00  2.00000000000e+00
5     2.33333333333e+00  2.33333333333e+00
6     2.66666666667e+00  2.66666666667e+00
7     3.00000000000e+00  3.00000000000e+00
8     3.33333333333e+00  3.33333333333e+00
9     3.66666666667e+00  3.66666666667e+00
10    4.00000000000e+00  4.00000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```

### User Function Defined
```bash
[kshumil@login02 shumilov_inlab02]$ ./shumilov_inlab02 -m 10 -x 1 2 3 4 --user-func
================================================================================
NE 591 Inlab #02: Lagrange Interpolation I/O
Author: Kirill Shumilov
Date: 01/17/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
#samples: m = 10
#points : n = 4
user-defined function: true
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
 i                    x                                    f(x)
--------------------------------------------------------------------------------
   1                    1.000000000000E+00          3.095598756531E+01
   2                    2.000000000000E+00          1.665436331219E+00
   3                    3.000000000000E+00          1.741559254738E-03
   4                    4.000000000000E+00         -8.516690103745E-06
--------------------------------------------------------------------------------
Where
i    : index of the interpolated point
x    : position of the interpolated point
f(x) : either user-supplied y-values or values from y = f(x) from the function
================================================================================
                                    Results
--------------------------------------------------------------------------------
 i           x                L(x)               f(x)               E(x)
--------------------------------------------------------------------------------
1     1.00000000000e+00  3.09559875653e+01  3.09559875653e+01  0.00000000000e+00
2     1.33333333333e+00  1.65200474380e+01  1.64270447078e+01 -9.30027302026e-02
3     1.66666666667e+00  7.07708482624e+00  6.18910067970e+00 -8.87984146547e-01
4     2.00000000000e+00  1.66543633122e+00  1.66543633122e+00  0.00000000000e+00
5     2.33333333333e+00 -6.76561445790e-01  3.12390416945e-01  9.88951862735e-01
6     2.66666666667e+00 -9.10571903530e-01  3.73128900646e-02  9.47884793595e-01
7     3.00000000000e+00  1.74155925474e-03  1.74155925474e-03  0.00000000000e+00
8     3.33333333333e+00  1.09871554382e+00 -2.84810271715e-04 -1.09900035409e+00
9     3.66666666667e+00  1.41868665142e+00 -7.26490243131e-05 -1.41875930044e+00
10    4.00000000000e+00 -8.51669010374e-06 -8.51669010374e-06  0.00000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```