add_executable(outlab01 main.cxx)

target_link_libraries(outlab01 PUBLIC methods ne591_compiler_flags fmt argparse)