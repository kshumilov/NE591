add_library(methods INTERFACE)
target_include_directories(methods INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/methods)
target_link_libraries(methods INTERFACE ne591_compiler_flags fmt)

#add_subdirectory(inlab01)
add_subdirectory(outlab01)

