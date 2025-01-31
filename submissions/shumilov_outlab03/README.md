# Outlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.26
```

# File Tree
The project `main.cxx` function is located under `src`
```
shumilov_outlab03  <-- Root Directory (Run commands from this directory)
├── CMakeLists.txt    <-- Compilation Script (do not edit)
├── README.md   <-- Instructions
├── analysis   <-- Analysis and report
│   ├── Analsis.ipynb   <-- Code that generates plots and report
│   ├── Analsis.pdf   <-- PDF version of the code that generates plots and report
│   └── data
│       ├── part_4.csv
│       └── part_5.csv
├── external   <-- Location of user-defined function
│   ├── CMakeLists.txt
│   ├── user_func.cpp   <-- Definition of the user-defined function (EDIT HERE)
│   └── user_func.h
├── include
│   ├── array.h
│   ├── integrate  
│   │   ├── gauss.h  <-- Gauss-Legrange Integration and Nodes
│   │   ├── integrate.h   <-- Main Integration Module
│   │   ├── simpson.h
│   │   └── trapezoidal.h
│   ├── interpolate.h
│   ├── legendre.h   <-- Gauss-Legrange Polynomials and Roots (using Newton-Raphson)
│   ├── matrix.h
│   ├── optimize.h  <-- Tempalte for fixed-point iteration
│   └── roots.h  <-- Implementation of Newton-Raphon root-finding
└── src
    └── main.cxx
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
[kshumil@login03 shumilov_outlab03]$ module load gcc/13.2.0
[kshumil@login03 shumilov_outlab03]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (4.8s)
-- Generating done (0.1s)
-- Build files have been written to: /home/kshumil/shumilov_outlab03/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target outlab03 -- -j
```

### Example
```bash
[kshumil@login02 shumilov_outlab03]$ cmake --build build --config Release --target outlab03 -- -j
[ 25%] Building CXX object external/CMakeFiles/user_func.dir/user_func.cpp.o
[ 50%] Linking CXX shared library libuser_func.so
[ 50%] Built target user_func
[ 75%] Building CXX object CMakeFiles/outlab03.dir/src/main.cxx.o
[100%] Linking CXX executable shumilov_outlab03
[100%] Built target outlab03
```

Note, that the warnings are irrelevant to the validity of this code.

At this point the executable can be found in:
```bash
[kshumil@login03 shumilov_outlab03]$ cd ./shumilov_outlab03 -h
================================================================================
NE 591 Inlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/24/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
Usage: shumilov_outlab03 [--help] --start VAR --end VAR --subintervals VAR [--quad VAR]

Optional arguments:
  -h, --help          shows help message and exits
  -a, --start         Start of interval integration [required]
  -b, --end           End of interval integration [required]
  -m, --subintervals  Number of subintervals [required]
  --quad              Quadrature type (trap, simp, gauss) nargs=0..1] [default: "trap"]
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
[kshumil@login02 shumilov_outlab03]$ vim external/user_func.cpp
[kshumil@login02 shumilov_outlab03]$ cmake --build build --config Release --target user_func -- -j
[ 50%] Building CXX object external/CMakeFiles/user_func.dir/user_func.cpp.o
[100%] Linking CXX shared library libuser_func.so
[100%] Built target user_func
```

# Examples
## Example Trapezoidal
```bash
[kshumil@login03 outlab03]$ ./shumilov_outlab03 -m 10 -a 1.0 -b 5.0 --quad trap
================================================================================
NE 591 Outlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/31/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Quadrature.............: Trapezoidal
================================================================================
                                    Results
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Number of Points       : 11
Quadrature.............: Trapezoidal
Integral...............: 147.632315031182
================================================================================
```

## Example Simpson
```bash
[kshumil@login03 outlab03]$ ./shumilov_outlab03 -m 10 -a 1.0 -b 5.0 --quad simp
================================================================================
NE 591 Outlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/31/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
================================================================================
Input Arguments
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Number of Points       : 11
Quadrature.............: Simpson
================================================================================
Results
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Number of Points       : 11
Quadrature.............: Simpson
Integral...............: 145.715210160708
================================================================================
```

## Example Gauss-Legendre
```bash
[kshumil@login03 outlab03]$ ./shumilov_outlab03 -m 10 -a 1.0 -b 5.0 --quad gauss
================================================================================
NE 591 Outlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/31/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
================================================================================
                                Input Arguments                                 
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Number of Points       : 11
Quadrature.............: Gauss-Legendre
================================================================================
                                  Gauss Nodes                                   
--------------------------------------------------------------------------------
 i          x_i                w_i        
--------------------------------------------------------------------------------
1    -9.78228658146e-01  5.56685671162e-02
2    -8.87062599768e-01  1.25580369465e-01
3    -7.30152005574e-01  1.86290210928e-01
4    -5.19096129207e-01  2.33193764592e-01
5    -2.69543155952e-01  2.62804544510e-01
6     1.23259516441e-32  2.72925086778e-01
7     2.69543155952e-01  2.62804544510e-01
8     5.19096129207e-01  2.33193764592e-01
9     7.30152005574e-01  1.86290210928e-01
10    8.87062599768e-01  1.25580369465e-01
11    9.78228658146e-01  5.56685671162e-02
================================================================================
                                    Results                                     
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Quadrature.............: Gauss-Legendre
Integral...............: 72.847438637059
================================================================================
```

## Example Run in Interactive mode
```bash
[kshumil@login03 outlab03]$  ./shumilov_outlab03
================================================================================
NE 591 Outlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/24/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
Enter interval start:
1
Enter interval end:
5
Enter interval number of intervals:
10
Enter type of quadrature 0/1/2 (Trapezoidal/Simpson/Gauss):
1
================================================================================
Input Arguments
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Quadrature.............: Simpson
================================================================================
Results
--------------------------------------------------------------------------------
Integration Interval...: [1, 5]
Integration Step.......: 0.4
Number of Subintervals.: 10
Quadrature.............: Simpson
Integral...............: 145.715210160708
================================================================================
```
