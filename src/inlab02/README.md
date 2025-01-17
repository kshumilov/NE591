# Inlab #02
- Author: Kirill Shumilov
- Date: 01/17/2024

# Requirements
- C++ Compiler: GCC/Clang supporing C++23 standard
- CMake of version $>=3.26$

## Hazel HPC System (NCSU)
Make sure you are logged to the cluser with
```bash
ssh -X $USER:login.hpc.ncsu.edu
```

Request an interactive session on a compute node:
```bash
bsub -Is -n 4 -R "span[hosts=1]" -W 20 bash
```

Load the latest gcc compiler:
```bash
module load gcc/13.2.0
```

# Building
To build a desired inlab or outlab, from the root of the project execute the following commands:
```bash
cmake -S. -Bbuild --config Release
cmake --build build --target outlab01 -j
cmake --install build --prefix <install_location>
```
Where `<install_location>` is the location of `bin`, `include`, and `lib` directories to be installed.
The binary is located in `bin` directory.

To run the desired project:
```bash
<install_location>/bin/shumilov_outlabNN [ARGS...]
```

# Example run
```bash
> ./bin/shumilov_outlab01 -k 2.0 -M 3 -N 4 -J 5
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

```