#!/bin/bash

module purge
module load cmake/3.24
module load gcc/13.2.0
module load mpi/mpich-x86_64

# rm -rf build/
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release -DMPI_CXX_COMPILER=mpic++
cmake --build build/ -- -j
