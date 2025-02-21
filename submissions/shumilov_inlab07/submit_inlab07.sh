#!/bin/bash

#BSUB -J shumilov_inlab07
#BSUB -o stdout.%J
#BSUB -e stderr.%J
#BSUB -W 00:1
#BSUB -n 2

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

mpirun -n 2 ./shumilov_inlab07
