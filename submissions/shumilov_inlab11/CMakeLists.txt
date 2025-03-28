cmake_minimum_required(VERSION 3.24)

project(
        ne591
        VERSION 1.0.0
        DESCRIPTION "Implementation for NE 591 Math & Comp (Spring 2025): Project #01"
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
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-ffast-math>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3;>>"
)

# Fetch Dependencies
include(FetchContent)

## Fetch Argparse -- parsing command line arguments
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG 68fd0277eea5412aff9b91c0b70efc698359dae0
)

## Fetch fmt -- better formatting
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        8303d140a1a11f19b982a9f664bbe59a1ccda3f4
)

## Fetch json -- easy output handling
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

## Make the dependencies available
FetchContent_MakeAvailable(argparse fmt json)

add_subdirectory(include)

# Executables

## Main Executable
add_executable(inlab11 ./src/inlab11.cpp)
target_link_libraries(
        inlab11
        PRIVATE
        methods
        ne591_compiler_flags  # Compilation Flags
        argparse              # CLI argument parsing
        fmt::fmt              # Formatting
        nlohmann_json::nlohmann_json  # Json IO
)
target_include_directories(inlab11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lab_include)

set_property(TARGET inlab11 PROPERTY OUTPUT_NAME shumilov_inlab11)
add_custom_command(TARGET inlab11
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab11> ${CMAKE_CURRENT_SOURCE_DIR}
)

# Contains definition for CMAKE_INSTALL_DATADIR
# Only needs to be included once
include(GNUInstallDirs)
install(TARGETS inlab11 DESTINATION bin OPTIONAL)
