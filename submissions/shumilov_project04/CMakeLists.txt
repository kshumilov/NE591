cmake_minimum_required(VERSION 3.24)

project(
        ne591
        VERSION 1.0.0
        DESCRIPTION "Implementations for NE 591 Math & Comp (Spring 2025)"
        LANGUAGES CXX
)

# specify the C++ standard
add_library(ne591_compiler_flags INTERFACE)
target_compile_features(ne591_compiler_flags INTERFACE cxx_std_23)

# add compiler warning flags just when building this project via
# the BUILD_INTERFACE genex
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(ne591_compiler_flags INTERFACE
        "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic;-Wshadow;-Wformat=2;-Wunused;-march=native;-ffast-math>>"
        "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

include(FetchContent)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG 68fd0277eea5412aff9b91c0b70efc698359dae0
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        8303d140a1a11f19b982a9f664bbe59a1ccda3f4
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(argparse fmt json)

# Installed Dependencies
find_package(MPI REQUIRED)
message(STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")

add_subdirectory(include)

add_executable(project04 ./src/project.cpp ./src/mpi_utils.cpp)
target_link_libraries(project04 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json MPI::MPI_CXX)
target_include_directories(project04 PRIVATE ./lab_include)
set_property(TARGET project04 PROPERTY OUTPUT_NAME shumilov_project04)
add_custom_command(TARGET project04
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:project04> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS project04 DESTINATION bin OPTIONAL)
