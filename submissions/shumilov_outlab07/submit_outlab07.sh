#!/bin/bash

#BSUB -J shumilov_outlab07
#BSUB -o stdout.%J
#BSUB -e stderr.%J
#BSUB -W 00:10
#BSUB -n 4

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

mpirun -n 4 ./shumilov_outlab07
