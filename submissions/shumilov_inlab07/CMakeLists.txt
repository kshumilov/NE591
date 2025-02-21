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

# Locate MPI
find_package(MPI REQUIRED)

add_executable(lab07 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(lab07 PRIVATE MPI::MPI_CXX
        fmt::fmt)

set_property(TARGET lab07 PROPERTY OUTPUT_NAME shumilov_inlab07)

add_custom_command(TARGET lab07
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lab07> ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS lab07 DESTINATION bin OPTIONAL)
