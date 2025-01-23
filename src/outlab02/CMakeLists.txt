add_executable(outlab02 main.cxx)

target_link_libraries(outlab02 PRIVATE methods ne591_compiler_flags fmt::fmt-header-only argparse)

set_property(TARGET outlab02 PROPERTY OUTPUT_NAME shumilov_outlab02)

install(TARGETS outlab02 DESTINATION bin OPTIONAL)