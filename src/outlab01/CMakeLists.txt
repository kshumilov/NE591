add_executable(outlab01 main.cxx)

target_link_libraries(outlab01 PRIVATE methods ne591_compiler_flags fmt::fmt-header-only argparse)

set_target_properties(outlab01 PROPERTIES
        OUTPUT_NAME shumilov_outlab01
)

install(TARGETS outlab01 DESTINATION bin OPTIONAL)