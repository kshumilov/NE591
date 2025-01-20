# Outlab #01
- Author: Kirill Shumilov

# Requirements
- C++ Compiler: GCC/Clang supporing C++23 standard
- CMake of version $>=3.26$

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
To build `outlab01` run the following commands from the root of the project `<NE591>`:

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
cmake --build build --config Release --target outlab01 -j
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
./build/src/outlab01/shumilov_outlab01 -h
```

### Example
```bash
[kshumil@login02 NE591]$ ./build/src/outlab01/shumilov_outlab01 -h
Usage: ./build/src/outlab01/shumilov_outlab01 [--help] -k VAR -M VAR -N VAR -J VAR

================================================================================
NE 591 Outlab #01: Basic Math
Author: Kirill Shumilov
================================================================================
This program showcases the use of matrix arithmetic,
such as matrix-matrix addition and multiplication and matrix-scalar
multiplication

Optional arguments:
  -h, --help  shows help message and exits
  -k          Scalar multiplier of matrix A (a real number) [required]
  -M          #rows for matrices A and B (positive integer) [required]
  -N          #rows for matrix F,
              #cols for matrices A and B (positive integer) [required]
  -J          #cols for matrix F (positive integer) [required]
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
which will create `./bin/shumilov_outlab02`.


# Running
## Request a compute Compute Node on Hazel
Request an interactive session on a compute node:
```bash
bsub -Is -n 4 -R "span[hosts=1]" -W 20 bash
```

## Run the project
To run the desired project:
```bash
<install_location>/bin/shumilov_inlab02 [ARGS...]
```

# Example build and Run
## Build and Run on Login Node
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
-- Configuring done (4.2s)
-- Generating done (0.0s)
-- Build files have been written to: /home/kshumil/ne591/NE591/build
[kshumil@login03 NE591]$ cmake --build build --target outlab01 --config Release -j
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object src/outlab01/CMakeFiles/outlab01.dir/main.cxx.o
[100%] Linking CXX executable shumilov_outlab01
[100%] Built target outlab01
[kshumil@login03 NE591]$ ./build/src/outlab01/shumilov_outlab01 -h
Usage: ./build/src/outlab01/shumilov_outlab01 [--help] -k VAR -M VAR -N VAR -J VAR

================================================================================
NE 591 Outlab #01: Basic Math
Author: Kirill Shumilov
================================================================================
This program showcases the use of matrix arithmetic,
such as matrix-matrix addition and multiplication and matrix-scalar
multiplication

Optional arguments:
  -h, --help  shows help message and exits
  -k          Scalar multiplier of matrix A (a real number) [required]
  -M          #rows for matrices A and B (positive integer) [required]
  -N          #rows for matrix F,
              #cols for matrices A and B (positive integer) [required]
  -J          #cols for matrix F (positive integer) [required]
[kshumil@login03 NE591]$ ./build/src/outlab01/shumilov_outlab01 -k 2.0 -M 3 -N 4 -J 5
================================================================================
NE 591 Outlab #01: Basic Math
Author: Kirill Shumilov
================================================================================
This program showcases the use of matrix arithmetic,
such as matrix-matrix addition and multiplication and matrix-scalar
multiplication
--------------------------------------------------------------------------------
Input Arguments:
k = 2
M = 3
N = 4
J = 5
--------------------------------------------------------------------------------
A<3 x 4, d> where a_{m,n} = 1.0 if (m == n) else 0.5
[[  1.00   0.50   0.50   0.50]
 [  0.50   1.00   0.50   0.50]
 [  0.50   0.50   1.00   0.50]]

B<3 x 4, d> where b_{m,n} = 0.25 if (m <= n) else 0.75
[[  0.25   0.25   0.25   0.25]
 [  0.75   0.25   0.25   0.25]
 [  0.75   0.75   0.25   0.25]]

F<4 x 5, d> where f_{n,j} = 1.0 / (n + j)
[[  0.50   0.33   0.25   0.20   0.17]
 [  0.33   0.25   0.20   0.17   0.14]
 [  0.25   0.20   0.17   0.14   0.12]
 [  0.20   0.17   0.14   0.12   0.11]]

C<3 x 4, d> = A + B
[[  1.25   0.75   0.75   0.75]
 [  1.25   1.25   0.75   0.75]
 [  1.25   1.25   1.25   0.75]]

D<3 x 4, d>  = k * A
[[  2.00   1.00   1.00   1.00]
 [  1.00   2.00   1.00   1.00]
 [  1.00   1.00   2.00   1.00]]

E<3 x 5, d> = A * F
[[  0.75   0.64   0.56   0.50   0.42]
 [  0.58   0.64   0.56   0.50   0.42]
 [  0.58   0.55   0.56   0.50   0.42]]

[kshumil@login03 NE591]$
```

## Build on Login Node and Run on Compute node
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
-- Configuring done (4.7s)
-- Generating done (0.0s)
-- Build files have been written to: /home/kshumil/ne591/NE591/build
[kshumil@login03 NE591]$ cmake --build build --target outlab01 --config Release -j
[ 20%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 40%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 60%] Linking CXX static library libfmt.a
[ 60%] Built target fmt
[ 80%] Building CXX object src/outlab01/CMakeFiles/outlab01.dir/main.cxx.o
[100%] Linking CXX executable shumilov_outlab01
[100%] Built target outlab01
[kshumil@login03 NE591]$ bsub -Is -n 4 -R "span[hosts=1]" -W 20 bash
Job <287781> is submitted to default queue <debug>.
<<Waiting for dispatch ...>>
<<Starting on c031n01>>
[kshumil@c031n01 NE591]$ ./build/src/outlab01/shumilov_outlab01 -k 0.1 -M 3 -N 4 -J 5
================================================================================
NE 591 Outlab #01: Basic Math
Author: Kirill Shumilov
================================================================================
This program showcases the use of matrix arithmetic,
such as matrix-matrix addition and multiplication and matrix-scalar
multiplication
--------------------------------------------------------------------------------
Input Arguments:
k = 0.1
M = 3
N = 4
J = 5
--------------------------------------------------------------------------------
A<3 x 4, d> where a_{m,n} = 1.0 if (m == n) else 0.5
[[  1.00   0.50   0.50   0.50]
 [  0.50   1.00   0.50   0.50]
 [  0.50   0.50   1.00   0.50]]

B<3 x 4, d> where b_{m,n} = 0.25 if (m <= n) else 0.75
[[  0.25   0.25   0.25   0.25]
 [  0.75   0.25   0.25   0.25]
 [  0.75   0.75   0.25   0.25]]

F<4 x 5, d> where f_{n,j} = 1.0 / (n + j)
[[  0.50   0.33   0.25   0.20   0.17]
 [  0.33   0.25   0.20   0.17   0.14]
 [  0.25   0.20   0.17   0.14   0.12]
 [  0.20   0.17   0.14   0.12   0.11]]

C<3 x 4, d> = A + B
[[  1.25   0.75   0.75   0.75]
 [  1.25   1.25   0.75   0.75]
 [  1.25   1.25   1.25   0.75]]

D<3 x 4, d>  = k * A
[[  0.10   0.05   0.05   0.05]
 [  0.05   0.10   0.05   0.05]
 [  0.05   0.05   0.10   0.05]]

E<3 x 5, d> = A * F
[[  0.75   0.64   0.56   0.50   0.42]
 [  0.58   0.64   0.56   0.50   0.42]
 [  0.58   0.55   0.56   0.50   0.42]]

[kshumil@c031n01 NE591]$
```