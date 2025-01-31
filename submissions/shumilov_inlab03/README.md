# Outlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
```requirements
- C++ Compiler: GCC/Clang supporing C++23 standard
- CMake >=3.26
```

# File Tree
The project `main.cxx` function is located under `src`
```
shumilov_inlab03  <-- Root Directory (Run commands from this directory)
├── CMakeLists.txt   <-- Compilation Script (do not edit)
├── README.md        <-- Instructions
├── external   <-- Location of user-defined function
│   ├── CMakeLists.txt
│   ├── user_func.cpp   <-- Definition of the user-defined function (EDIT HERE)
│   └── user_func.h
├── include
│   ├── array.h   <-- Utility functions for working with arrays
│   └── integrate.h   <-- Integration methods
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
[kshumil@login03 shumilov_inlab03]$ module load gcc/13.2.0
[kshumil@login03 shumilov_inlab03]$ cmake -S. -Bbuild
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
-- Build files have been written to: /home/kshumil/shumilov_inlab03/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target inlab03 -- -j
```

### Example
```bash
[ 25%] Building CXX object external/CMakeFiles/user_func.dir/user_func.cpp.o
[ 50%] Linking CXX shared library libuser_func.so
[ 50%] Built target user_func
[ 75%] Building CXX object CMakeFiles/inlab03.dir/src/main.cxx.o
In file included from /home/kshumil/shumilov_inlab03/src/main.cxx:10:
/home/kshumil/shumilov_inlab03/include/integrate.h: In instantiation of ‘scalar_t gauss(std::span<const scalar_t>, scalar_t) [with scalar_t = double]’:
/home/kshumil/shumilov_inlab03/include/integrate.h:58:25:   required from ‘scalar_t integrate(std::span<const scalar_t>, Quadrature, scalar_t) [with scalar_t = double]’
/home/kshumil/shumilov_inlab03/src/main.cxx:222:50:   required from here
/home/kshumil/shumilov_inlab03/include/integrate.h:41:38: warning: unused parameter ‘y’ [-Wunused-parameter]
   41 | auto gauss(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
      |            ~~~~~~~~~~~~~~~~~~~~~~~~~~^
/home/kshumil/shumilov_inlab03/include/integrate.h:41:56: warning: unused parameter ‘step’ [-Wunused-parameter]
   41 | auto gauss(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
      |                                         ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~
[100%] Linking CXX executable shumilov_inlab03
[100%] Built target inlab03
```

Note, that the warnings are irrelevant to the validity of this code.

At this point the executable can be found in:
```bash
[kshumil@login03 shumilov_inlab03]$ cd ./shumilov_inlab03 -h
================================================================================
NE 591 Inlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/24/2025
================================================================================
This program performs composite numerical integration of user-defined function
using three quadratures: Trapezoidal, Simpson, and Gauss
Usage: shumilov_inlab03 [--help] --start VAR --end VAR --subintervals VAR [--quad VAR]

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
[kshumil@login02 shumilov_inlab03]$ vim external/user_func.cpp
[kshumil@login02 shumilov_inlab03]$ cmake --build build --config Release --target user_func -- -j
[ 50%] Building CXX object external/CMakeFiles/user_func.dir/user_func.cpp.o
[100%] Linking CXX shared library libuser_func.so
[100%] Built target user_func
```

# Example Run
```bash
[kshumil@login03 inlab03]$ ./shumilov_inlab03 -m 10 -a 1.0 -b 5.0 --quad trap
================================================================================
NE 591 Inlab #03: Numerical Integration
Author: Kirill Shumilov
Date: 01/24/2025
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
Quadrature.............: Trapezoidal
Integrals..............: 147.632315031182
================================================================================
```

# Example Run in Interactive mode
```bash
[kshumil@login03 inlab03]$  ./shumilov_inlab03
================================================================================
NE 591 Inlab #03: Numerical Integration
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
