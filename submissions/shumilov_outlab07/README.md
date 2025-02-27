# Outlab #07
- Author: Kirill Shumilov
- Date: 02/21/2025

# Requirements
```requirements
C++ Compiler: GCC/Clang supporing C++23 standard
CMake >=3.24
```

# Compilation
To compile make sure that `compile_inlab07.sh` has executable privileges and run:
```bash
./compile_inlab07.sh
```
Remember to compile on the login node first.

# Running the script
To modify the `n` modify `input` to include the desired positive integer to sum.

## Login Node
On the login node (not recommended), make sure the these modules are load
```bash
module load gcc/13.2.0
module load mpi/mpich-x86_64
```
and then execute:
```bash
mpirun -n 4 ../shumilov_outlab07
```

## Submitting the script to Scheduler
To submit the script to job scheduler, please run:
```bash
bsub < ./submit_outlab07.sh
```
**AFTER** compiling on the login node.

