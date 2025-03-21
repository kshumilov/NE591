#!/bin/bash

#BSUB -J shumilov_project04
#BSUB -o stdout.%J
#BSUB -e stderr.%J
#BSUB -W 00:10
#BSUB -n 9

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

mpirun -n 9 ./shumilov_project04 examples/s12_pj.inp examples/s12_pj.out
