add_executable(outlab01 main.cxx)

target_link_libraries(outlab01 PUBLIC methods ne591_compiler_flags fmt argparse)

set_property(TARGET outlab01 PROPERTY OUTPUT_NAME shumilov_outlab01)

install(TARGETS outlab01 DESTINATION bin)