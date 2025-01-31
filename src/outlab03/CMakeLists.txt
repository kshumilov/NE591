add_executable(outlab03 main.cxx)
add_custom_command(TARGET outlab03
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab03> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(outlab03 PRIVATE methods ne591_compiler_flags fmt::fmt-header-only argparse)

set_property(TARGET outlab03 PROPERTY OUTPUT_NAME shumilov_outlab03)

install(TARGETS outlab03 DESTINATION bin OPTIONAL)