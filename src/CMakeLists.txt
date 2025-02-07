add_library(methods INTERFACE)
target_include_directories(methods INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/methods ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_link_libraries(methods INTERFACE ne591_compiler_flags fmt::fmt-header-only)

#add_subdirectory(inlab01)
add_subdirectory(outlab01)

add_subdirectory(inlab02)
add_subdirectory(outlab02)

add_subdirectory(inlab03)
add_subdirectory(outlab03)

add_subdirectory(inlab04)
add_subdirectory(outlab04)

add_subdirectory(inlab05)

