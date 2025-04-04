#!/bin/bash

#BSUB -J shumilov_project05
#BSUB -o stdout.%J
#BSUB -e stderr.%J
#BSUB -W 00:10
#BSUB -n 9
#BSUB -R span[hosts=1]

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

mpirun -n $LSB_DJOB_NUMPROC ./shumilov_project05 examples/s12_pj.inp 1> examples/s12_pj.out
