# Outlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
```requirements
- C++ Compiler: GCC/Clang supporing C++23 standard
- CMake >=3.26
```

# File Tree
The project `main.cpp` function is located under `<project_root>/src/inlab03`
```bash
<NE591>/src/inlab03
├── CMakeLists.txt
├── main.cxx
├── README.md
├── shumilov_inlab03
└── user_func.h
```
While method implementation is located in:
```bash
NE591/src/methods/
├── array.h
├── integrate.h
├── interpolate.h
└── matrix.h
```
# Building
To build `outlab03` run the following commands from the root of the project `<NE591>`:

## 0. Hazel HPC System (NCSU)
Make sure you are logged to the cluser with
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
[kshumil@login03 NE591]$ module load gcc/13.2.0
[kshumil@login03 NE591]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (6.0s)
-- Generating done (0.3s)
-- Build files have been written to: /home/kshumil/ne591/NE591/build```
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target inlab03 -- -j
```

### Example
```bash
[kshumil@login03 NE591]$ cmake --build build --config Release --target inlab03 -- -j
[ 50%] Building CXX object src/inlab03/CMakeFiles/inlab03.dir/main.cxx.o
In file included from /home/kshumil/ne591/NE591/src/inlab03/main.cxx:10:
/home/kshumil/ne591/NE591/src/methods/integrate.h: In instantiation of ‘scalar_t gauss(std::span<const scalar_t>, scalar_t) [with scalar_t = double]’:
/home/kshumil/ne591/NE591/src/methods/integrate.h:58:25:   required from ‘scalar_t integrate(std::span<const scalar_t>, Quadrature, scalar_t) [with scalar_t = double]’
/home/kshumil/ne591/NE591/src/inlab03/main.cxx:182:46:   required from here
/home/kshumil/ne591/NE591/src/methods/integrate.h:41:38: warning: unused parameter ‘y’ [-Wunused-parameter]
   41 | auto gauss(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
      |            ~~~~~~~~~~~~~~~~~~~~~~~~~~^
/home/kshumil/ne591/NE591/src/methods/integrate.h:41:56: warning: unused parameter ‘step’ [-Wunused-parameter]
   41 | auto gauss(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
      |                                         ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~
[100%] Linking CXX executable shumilov_inlab03
[100%] Built target inlab03
```

Note, that the warnings are irrelevant to the validity of this code.

At this point the executable can be found in:
```bash
[kshumil@login03 NE591]$ cd ./src/inlab03/
[kshumil@login03 inlab03]$ ./shumilov_inlab03 -h
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
If the user wishes to supply their own custom function, they should edit `user_func.h` header file.
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
    return std::exp(x);
}
```
The program must be recompiled after `user_func.h` has been edited

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
