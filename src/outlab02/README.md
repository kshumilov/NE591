# Outlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
- C++ Compiler: GCC/Clang supporing C++23 standard
- CMake of version $>=3.26$

# File Tree
The project `main.cpp` function is located under `<project_root>/src/outlab02`
```
<project_root>/src/outlab02
├── analysis
│   ├── data
│   │   ├── n3
│   │   │   ├── input
│   │   │   ├── input.csv
│   │   │   ├── output.csv
│   │   │   └── plot.png
│   │   ├── n4
│   │   │   ├── input
│   │   │   ├── input.csv
│   │   │   ├── output.csv
│   │   │   └── plot.png
│   │   └── n8
│   │       ├── input
│   │       ├── input.csv
│   │       ├── output.csv
│   │       └── plot.png
│   └── report.ipynb
├── CMakeLists.txt
├── main.cxx
├── README.md
└── user_func.h
```

While the implementation for Lagrange Interpolation Polynomial is located under `<project_root>/src/methods/` 
```
<project_root>/src/methods/
├── array.h
├── interpolate.h   <--- Here
└── matrix.h
```

# Hazel HPC System (NCSU)
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
[kshumil@login03 NE591]$ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 13.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/local/apps/gcc/13.2.0/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- {fmt} version: 11.1.2
-- Build type:
-- Configuring done (4.9s)
-- Generating done (0.2s)
-- Build files have been written to: /home/kshumil/ne591/NE591/build
```

## 2. Compile the code
Now, compile the code:
```bash
cmake --build build --config Release --target outlab02 -- -j
```

### Example
```bash
[kshumil@login03 NE591]$ cmake --build build --config Release --target outlab02 -- -j
[ 50%] Building CXX object src/outlab02/CMakeFiles/outlab02.dir/main.cxx.o
/home/kshumil/ne591/NE591/src/outlab02/main.cxx: In function ‘std::tuple<Parameters, std::optional<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::optional<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > > parse_and_validate(int, char**)’:
/home/kshumil/ne591/NE591/src/outlab02/main.cxx:236:28: warning: unused variable ‘i’ [-Wunused-variable]
  236 |             for (const int i : std::views::iota(0, params.n)) {
      |                            ^
/home/kshumil/ne591/NE591/src/outlab02/main.cxx:253:28: warning: unused variable ‘i’ [-Wunused-variable]
  253 |             for (const int i : std::views::iota(0, params.n))
      |                            ^
[100%] Linking CXX executable shumilov_outlab02
[100%] Built target outlab02
```

Note, that the warnings are irrelevant to the validity of this code.

At this point the executable can be found in:
```bash
[kshumil@login03 NE591]$ cd ./src/outlab02/
[kshumil@login03 outlab02]$ ./shumilov_outlab02 -h
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
Usage: shumilov_outlab02 [--help] [-n VAR] [--samples VAR] [--points VAR...]... [--values VAR...]... [--user-func] [--interactive] [--save-input VAR] [--output VAR]

Optional arguments:
  -h, --help     shows help message and exits
  -n             Number of interpolation points.
                 If `-x`, `-y` or `--input` are provided, takes first `n` points.
  -m, --samples  Number of samples to interpolate the function at
  -x, --points   Distinct real interpolation points in increasing order: {x_i} [nargs: 1 or more] [may be repeated]
  -y, --values   Function values at interpolation points, y_i = f(x_i).
                 Ignored when `--user-func` is provided [nargs: 1 or more] [may be repeated]
  --user-func    Toggle the use of user-defined function in user_func.h
  --interactive  Toggle interactive mode.
                 In this mode, the user is queried to enter missing values
  --save-input   Save the input to the file in csv format
  --output       Output filename (writes in csv format)
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

# Running
Please, find bellow the usage instructions:
```bash
[kshumil@login03 outlab02]$ ./shumilov_outlab02 -h
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
Usage: shumilov_outlab02 [--help] [-n VAR] [--samples VAR] [--points VAR...]... [--values VAR...]... [--user-func] [--interactive] [--save-input VAR] [--output VAR]

Optional arguments:
-h, --help     shows help message and exits
-n             Number of interpolation points.
               If `-x`, `-y` or `--input` are provided, takes first `n` points.
-m, --samples  Number of samples to interpolate the function at
-x, --points   Distinct real interpolation points in increasing order: {x_i} [nargs: 1 or more] [may be repeated]
-y, --values   Function values at interpolation points, y_i = f(x_i).
               Ignored when `--user-func` is provided [nargs: 1 or more] [may be repeated]
--user-func    Toggle the use of user-defined function in user_func.h
--interactive  Toggle interactive mode.
               In this mode, the user is queried to enter missing values
--save-input   Save the input to the file in csv format
--output       Output filename (writes in csv format)
```

## Example: Running code in non-interactive mode (no user func)
```bash
[kshumil@login03 outlab02]$ ./shumilov_outlab02 -m 10 -x -1.0 -0.5 0.0 0.5 1.0 -y 1.2 3.4 5.6 7.8 9.10
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
#samples: m = 10
#points : n = 5
user-defined function: false
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
 i                    x                                    f(x)
--------------------------------------------------------------------------------
   1                   -1.000000000000E+00          1.200000000000E+00
   2                   -5.000000000000E-01          3.400000000000E+00
   3                    0.000000000000E+00          5.600000000000E+00
   4                    5.000000000000E-01          7.800000000000E+00
   5                    1.000000000000E+00          9.100000000000E+00
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
1    -1.00000000000e+00  1.20000000000e+00
2    -7.77777777778e-01  2.21458619113e+00
3    -5.55555555556e-01  3.16424325560e+00
4    -3.33333333333e-01  4.11481481481e+00
5    -1.11111111111e-01  5.09702789209e+00
6     1.11111111111e-01  6.10649291267e+00
7     3.33333333333e-01  7.10370370370e+00
8     5.55555555556e-01  8.01403749428e+00
9     7.77777777778e-01  8.72775491541e+00
10    1.00000000000e+00  9.10000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```

## Example: Running code in interactive mode (no user func)
In interactive mode the inputs are read from `stdin` in the following order:
1. Number of interpolation points (single integer)
2. Number of sampled points (single integer)
3. List of interpolation points
4. list of values at interpolation points
```bash
[kshumil@login03 outlab02]$ ./shumilov_outlab02 --interactive
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
Enter number of interpolation points:
5
Enter number of sampled points:
10
Enter 5 interpolation points:
-1.0 -0.5 0.0 0.5 1.0
Enter 5 values of interpolation points:
1.2 3.4 5.6 7.8 9.10
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
#samples: m = 10
#points : n = 5
user-defined function: false
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
 i                    x                                    f(x)
--------------------------------------------------------------------------------
   1                   -1.000000000000E+00          1.200000000000E+00
   2                   -5.000000000000E-01          3.400000000000E+00
   3                    0.000000000000E+00          5.600000000000E+00
   4                    5.000000000000E-01          7.800000000000E+00
   5                    1.000000000000E+00          9.100000000000E+00
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
1    -1.00000000000e+00  1.20000000000e+00
2    -7.77777777778e-01  2.21458619113e+00
3    -5.55555555556e-01  3.16424325560e+00
4    -3.33333333333e-01  4.11481481481e+00
5    -1.11111111111e-01  5.09702789209e+00
6     1.11111111111e-01  6.10649291267e+00
7     3.33333333333e-01  7.10370370370e+00
8     5.55555555556e-01  8.01403749428e+00
9     7.77777777778e-01  8.72775491541e+00
10    1.00000000000e+00  9.10000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```
Some of the inputs can be piped-in if they are not present in optional arguments:
```bash
[kshumil@login03 outlab02]$ cat input
3
-1.0 0.0 1.0
1.2 3.4 5.6
[kshumil@login03 outlab02]$ cat input | ./shumilov_outlab02 -m 10 --interactive
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
Enter number of interpolation points:
Enter 3 interpolation points:
Enter 3 values of interpolation points:
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
#samples: m = 10
#points : n = 3
user-defined function: false
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
 i                    x                                    f(x)
--------------------------------------------------------------------------------
   1                   -1.000000000000E+00          1.200000000000E+00
   2                    0.000000000000E+00          3.400000000000E+00
   3                    1.000000000000E+00          5.600000000000E+00
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
1    -1.00000000000e+00  1.20000000000e+00
2    -7.77777777778e-01  1.68888888889e+00
3    -5.55555555556e-01  2.17777777778e+00
4    -3.33333333333e-01  2.66666666667e+00
5    -1.11111111111e-01  3.15555555556e+00
6     1.11111111111e-01  3.64444444444e+00
7     3.33333333333e-01  4.13333333333e+00
8     5.55555555556e-01  4.62222222222e+00
9     7.77777777778e-01  5.11111111111e+00
10    1.00000000000e+00  5.60000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```

## Example: Running in interactive-mode with user-defined function
```bash
[kshumil@login03 outlab02]$ cat input
3
-1.0 0.0 1.0
[kshumil@login03 outlab02]$ cat input | ./shumilov_outlab02 -m 10 --user-func --interactive
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
Enter number of interpolation points:
Enter 3 interpolation points:
================================================================================
                                Input Arguments
--------------------------------------------------------------------------------
#samples: m = 10
#points : n = 3
user-defined function: true
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
 i                    x                                    f(x)
--------------------------------------------------------------------------------
   1                   -1.000000000000E+00          3.678794411714E-01
   2                    0.000000000000E+00          1.000000000000E+00
   3                    1.000000000000E+00          2.718281828459E+00
--------------------------------------------------------------------------------
Where
i    : index of the interpolated point
x    : position of the interpolated point
f(x) : either user-supplied y-values or values from y = f(x) from the function
================================================================================
                                    Results
--------------------------------------------------------------------------------
 i           x                f(x)               L(x)               E(x)
--------------------------------------------------------------------------------
1    -1.00000000000e+00  3.67879441171e-01  3.67879441171e-01  0.00000000000e+00
2    -7.77777777778e-01  4.59425824036e-01  4.14484887733e-01  4.49409363027e-02
3    -5.55555555556e-01  5.73753420737e-01  5.14727927857e-01  5.90254928805e-02
4    -3.33333333333e-01  7.16531310574e-01  6.68608561543e-01  4.79227490311e-02
5    -1.11111111111e-01  8.94839316814e-01  8.76126788790e-01  1.87125280240e-02
6     1.11111111111e-01  1.11751906874e+00  1.13728260960e+00 -1.97635408583e-02
7     3.33333333333e-01  1.39561242509e+00  1.45207602397e+00 -5.64635988858e-02
8     5.55555555556e-01  1.74290899863e+00  1.82050703191e+00 -7.75980332721e-02
9     7.77777777778e-01  2.17662993172e+00  2.24257563340e+00 -6.59457016851e-02
10    1.00000000000e+00  2.71828182846e+00  2.71828182846e+00  0.00000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```

# Analysis
The report and the code that is used to generate it is located under `<project_root>/src/outlab02/analysis`.

The input files for the cases $n=3$, $n=4$, and $n=8$ is located under `<project_root>/src/outlab02/analysis/data`.

## Example:
```bash
[kshumil@login03 outlab02]$ cat analysis/data/n3/input
3
100
-1.0 0.0 1.0
[kshumil@login03 outlab02]$ cat analysis/data/n3/input | ./shumilov_outlab02 --interactive --user-func --output analysis/data/n3/output.csv
================================================================================
NE 591 OutLab #02: Lagrange Interpolation
Author: Kirill Shumilov
Date: 01/23/2025
================================================================================
This program perform Lagrange Interpolation of a 1D real function
Enter number of interpolation points:
Enter number of sampled points:
Enter 3 interpolation points:
================================================================================
Input Arguments
--------------------------------------------------------------------------------
#samples: m = 100
#points : n = 3
user-defined function: true
--------------------------------------------------------------------------------
                              Interpolation Points
--------------------------------------------------------------------------------
i                    x                                    f(x)
--------------------------------------------------------------------------------
1                   -1.000000000000E+00          3.678794411714E-01
2                    0.000000000000E+00          1.000000000000E+00
3                    1.000000000000E+00          2.718281828459E+00
--------------------------------------------------------------------------------
Where
i    : index of the interpolated point
x    : position of the interpolated point
f(x) : either user-supplied y-values or values from y = f(x) from the function
================================================================================
Results
--------------------------------------------------------------------------------
i           x                f(x)               L(x)               E(x)
--------------------------------------------------------------------------------
1    -1.00000000000e+00  3.67879441171e-01  3.67879441171e-01  0.00000000000e+00
2    -9.79797979798e-01  3.75386926935e-01  3.69899870464e-01  5.48705647081e-03
3    -9.59595959596e-01  3.83047621429e-01  3.72363585653e-01  1.06840357758e-02
4    -9.39393939394e-01  3.90864651256e-01  3.75270586740e-01  1.55940645163e-02
...
98    9.59595959596e-01  2.61064145567e+00  2.62780021992e+00 -1.71587642451e-02
99    9.79797979798e-01  2.66391802231e+00  2.67281938124e+00 -8.90135893189e-03
100   1.00000000000e+00  2.71828182846e+00  2.71828182846e+00  0.00000000000e+00
--------------------------------------------------------------------------------
Where
i    : index of the sampled point
x    : position of the sampled point
L(x) : interpolated value at x
f(x) : True value, based on user-defined function
E(x) : L(x) - f(x)
================================================================================
```