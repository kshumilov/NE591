# Inlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
- C++ Compiler: GCC/Clang supporing C++23 standard
- CMake of version $>=3.26$

# File Tree
```
<NE591>/src/inlab02
├── CMakeLists.txt
├── README.md
├── main.cxx
└── user_func.h
```

## Hazel HPC System (NCSU)
Make sure you are logged to the cluser with
```bash
ssh -X $USER:login.hpc.ncsu.edu
```

Load the latest gcc compiler:
```bash
module load gcc/13.2.0
```

# Building
To build `inlab02` run the following commands from the root of the project `<NE591>`:

## 1. Initialize the build directory
```bash
cmake -S. -Bbuild
```
### Example
```bash
[kshumil@login02 NE591]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (4.2s)
-- Generating done (0.0s)
-- Build files have been written to: /home/kshumil/ne591/NE591/build
```

## 2. Compile the code
```bash
cmake --build build --config Release -j
```

### Example
```bash
[kshumil@login02 NE591]$ cmake --build build --config Release  -j
[ 14%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 28%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 42%] Linking CXX static library libfmt.a
[ 42%] Built target fmt
[ 57%] Building CXX object src/outlab01/CMakeFiles/outlab01.dir/main.cxx.o
[ 71%] Building CXX object src/inlab02/CMakeFiles/inlab02.dir/main.cxx.o
[ 85%] Linking CXX executable shumilov_outlab01
[ 85%] Built target outlab01
[100%] Linking CXX executable shumilov_inlab02
[100%] Built target inlab02
```

At this point the executable can be found in:
```bash
./build/src/shumilov_inlab02/shumilov_inlab02 -h
```

### Example
```bash
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

## 3. Install (Optional)
```bash
cmake --install build --prefix <install_location>
```
Where `<install_location>` is the location of `bin`, `include`, and `lib` directories to be installed.
The binary is located in `bin` directory. For instance, you can run to install the binaries into a current work project.
```bash
cmake --install build --prefix .
```
which will create `./bin/shumilov_inlabl02`.

To run the desired project:
```bash
<install_location>/bin/shumilov_inlab02 [ARGS...]
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
    return std::sin(x) * std::exp(-x * x) * 100.0;
}
```
The program must be recompiled after `user_func.h` has been edited

# Running
Please, find bellow the usage instructions:
```bash
[kshumil@login03 NE591]$ ./build/src/inlab02/shumilov_inlab02 -h
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

## Example run
### No Func Definition
```bash
[kshumil@login03 NE591]$ ./build/src/inlab02/shumilov_inlab02 -m 10 -x 1 2 3 4 -y 1 2 3 4
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
user defined function: false
--------------------------------------------------------------------------------
                              Interpolation Points
 i                    x                                    f(x)
   1                    1.000000000000E+00          1.000000000000E+00
   2                    2.000000000000E+00          2.000000000000E+00
   3                    3.000000000000E+00          3.000000000000E+00
   4                    4.000000000000E+00          4.000000000000E+00
--------------------------------------------------------------------------------
Where
i    : index of the intepolated point
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
[kshumil@login03 NE591]$ ./build/src/inlab02/shumilov_inlab02 -m 10 -x 1 2 3 4 --user-func
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
user defined function: true
--------------------------------------------------------------------------------
                              Interpolation Points
 i                    x                                    f(x)
   1                    1.000000000000E+00          3.095598756531E+01
   2                    2.000000000000E+00          1.665436331219E+00
   3                    3.000000000000E+00          1.741559254738E-03
   4                    4.000000000000E+00         -8.516690103745E-06
--------------------------------------------------------------------------------
Where
i    : index of the intepolated point
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

# Example
## Build and Run on Login Node
```bash
[kshumil@login03 NE591]$ pwd -P
/gpfs_common/share01/ne591s25/kshumil/NE591
[kshumil@login03 NE591]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (4.4s)
-- Generating done (0.0s)
-- Build files have been written to: /home/kshumil/ne591/NE591/build
[kshumil@login03 NE591]$ cmake --build build  --config Release -j
[ 28%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 28%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 42%] Linking CXX static library libfmt.a
[ 42%] Built target fmt
[ 57%] Building CXX object src/inlab02/CMakeFiles/inlab02.dir/main.cxx.o
[ 71%] Building CXX object src/outlab01/CMakeFiles/outlab01.dir/main.cxx.o
[ 85%] Linking CXX executable shumilov_outlab01
[ 85%] Built target outlab01
[100%] Linking CXX executable shumilov_inlab02
[100%] Built target inlab02
[kshumil@login03 NE591]$ ./build/src/inlab02/shumilov_inlab02 -h
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
[kshumil@login03 NE591]$ ./build/src/inlab02/shumilov_inlab02 -m 10 -x -0.5 0.0 0.5 1.0 --user-func
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
   1                   -5.000000000000E-01         -3.733769848894E+01
   2                    0.000000000000E+00          0.000000000000E+00
   3                    5.000000000000E-01          3.733769848894E+01
   4                    1.000000000000E+00          3.095598756531E+01
--------------------------------------------------------------------------------
Where
i    : index of the intepolated point
x    : position of the interpolated point
f(x) : either user-supplied y-values or values from y = f(x) from the function
================================================================================
                                    Results
--------------------------------------------------------------------------------
 i           x                L(x)               f(x)               E(x)
--------------------------------------------------------------------------------
1    -5.00000000000e-01 -3.73376984889e+01 -3.73376984889e+01  0.00000000000e+00
2    -3.33333333333e-01 -2.75905279687e+01 -2.92786678946e+01 -1.68813992593e+00
3    -1.66666666667e-01 -1.46048826772e+01 -1.61351321394e+01 -1.53024946221e+00
4     0.00000000000e+00  0.00000000000e+00  0.00000000000e+00  0.00000000000e+00
5     1.66666666667e-01  1.46048826772e+01  1.61351321394e+01  1.53024946221e+00
6     3.33333333333e-01  2.75905279687e+01  2.92786678946e+01  1.68813992593e+00
7     5.00000000000e-01  3.73376984889e+01  3.73376984889e+01  0.00000000000e+00
8     6.66666666667e-01  4.22271568522e+01  3.96486590526e+01 -2.57849779965e+00
9     8.33333333333e-01  4.06396656729e+01  3.69608635523e+01 -3.67880212057e+00
10    1.00000000000e+00  3.09559875653e+01  3.09559875653e+01  0.00000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```

