# NE 591 Math \& Comp Methods (Sprint 2025, NCSU)

This codebase implements the methods and algorithms covered in the NE 591 course.
Implement by Kirill Shumilv (kshumil@ncsu.edu)

# Table of Contents
1. [File Tree](#file-tree)
2. [Requirements](#requirements)
3. [Build and Install](#build-and-install)

# File Tree
Below is shown the structure of this repository.
Each inlab and outlab has its own directory under `src/`.
```
.
├── CMakeLists.txt
├── README.md
└── src
    ├── CMakeLists.txt
    ├── inlab01
    │   ├── CMakeLists.txt
    │   └── main.cxx
    ├── methods
    │   ├── matrix
    │   │   └── matrix.hxx
    │   └── taylor
    └── outlab01
        ├── CMakeLists.txt
        ├── README.md
        └── main.cxx
```
The `methods` directory serves as the central library for implemented methods.

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

# Build and Install
To build a desired inlab or outlab, from the root of the project execute the following commands:
```bash
cmake -S. -Bbuild --config Release
cmake --build build --target <labNN> -j
cmake --install build --prefix <install_location>
```
Where `<labNN>` is the name of the desired subproject, for instance `lab01`, and `<install_location>` 
is the location of `bin`, `include`, and `lib` directories to be installed.
The binary is located in `bin` directory.

To run the desired project:
```bash
<install_location>/bin/shumilov_<labNN> [ARGS...]
```