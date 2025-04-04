#!/bin/bash

#BSUB -J n64_shumilov_project04
#BSUB -o stdout.%J
#BSUB -e stderr.%J
#BSUB -W 20
#BSUB -q short
#BSUB -n 64
#BSUB -R span[hosts=1]

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

nproc=$LSB_DJOB_NUMPROC

for s in 128 256 512 1024; do
    for i in {1..10}; do
      for a in pj gs sor; do
        echo "Working $a $s $i"
        mpirun --map-by numa --bind-to core -n $nproc ./shumilov_project05 analysis/tests/g${s}_${a}.inp  -o analysis/tests/g${s}_${a}.n${nproc}.$i.out --flux analysis/tests/g${s}_${a}.n${nproc}.$i.flux
        echo "Done"
      done
    done
done