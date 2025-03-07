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

add_executable(inlab08 ./src/inlab08.cpp)
target_link_libraries(inlab08 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
target_include_directories(inlab08 PRIVATE ./lab_include)
set_property(TARGET inlab08 PROPERTY OUTPUT_NAME shumilov_inlab08)
add_custom_command(TARGET inlab08
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab08> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS inlab08 DESTINATION bin OPTIONAL)

add_executable(outlab08 ./src/outlab08.cpp ./src/mpi_utils.cpp)
target_link_libraries(outlab08 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json MPI::MPI_CXX)
target_include_directories(outlab08 PRIVATE ./lab_include)
set_property(TARGET outlab08 PROPERTY OUTPUT_NAME shumilov_outlab08)
add_custom_command(TARGET outlab08
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab08> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS outlab08 DESTINATION bin OPTIONAL)

add_executable(gauss_quad ./src/gauss_quad.cpp)
target_link_libraries(gauss_quad PRIVATE methods ne591_compiler_flags fmt::fmt argparse)
target_include_directories(gauss_quad PRIVATE lab_include)
add_custom_command(TARGET gauss_quad
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gauss_quad> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS gauss_quad DESTINATION bin OPTIONAL)
