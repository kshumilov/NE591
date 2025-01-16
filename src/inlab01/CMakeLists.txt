add_executable(inlab01 ./main.cxx)

target_link_libraries(inlab01 PUBLIC ne591_compiler_flags argparse)
