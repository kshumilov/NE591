add_executable(inlab02 main.cxx)

target_link_libraries(inlab02 PRIVATE methods ne591_compiler_flags fmt::fmt-header-only argparse)

set_property(TARGET inlab02 PROPERTY OUTPUT_NAME shumilov_inlab02)

install(TARGETS inlab02 DESTINATION bin OPTIONAL)