#!/bin/bash

#BSUB -J n4_shumilov_project04
#BSUB -o stdout.%J
#BSUB -e stderr.%J
#BSUB -W 20
#BSUB -q short
#BSUB -n 4
#BSUB -R span[hosts=1]

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

mpirun -n 4 ./shumilov_project04 analysis/tests/g128_pj.inp 1> analysis/tests/g128_pj.n4.out
mpirun -n 4 ./shumilov_project04 analysis/tests/g256_pj.inp 1> analysis/tests/g256_pj.n4.out
mpirun -n 4 ./shumilov_project04 analysis/tests/g512_pj.inp 1> analysis/tests/g512_pj.n4.out
mpirun -n 4 ./shumilov_project04 analysis/tests/g1024_pj.inp 1> analysis/tests/g1024_pj.n4.out
