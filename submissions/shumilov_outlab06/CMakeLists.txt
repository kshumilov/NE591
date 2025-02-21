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

# Contains definition for CMAKE_INSTALL_DATADIR
# Only needs to be included once
include(GNUInstallDirs)

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

# Executables

## Main Executable
add_executable(outlab06 ./src/main.cxx)
target_include_directories(outlab06 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(outlab06 PRIVATE ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)

set_property(TARGET outlab06 PROPERTY OUTPUT_NAME shumilov_outlab06)
add_custom_command(TARGET outlab06
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab06> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS outlab06 DESTINATION bin OPTIONAL)


## Custom Matrix Builder and Evaluate
add_executable(custom_system ./src/custom_system.cxx)
target_include_directories(custom_system PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(custom_system PRIVATE ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)

add_custom_command(TARGET custom_system
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:custom_system> ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS custom_system DESTINATION bin OPTIONAL)


## Method Comparator
add_executable(compare_methods ./src/compare_methods.cxx)
target_include_directories(compare_methods PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(compare_methods PRIVATE ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)

add_custom_command(TARGET compare_methods
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:compare_methods> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS compare_methods DESTINATION bin OPTIONAL)

