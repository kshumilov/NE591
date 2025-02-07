add_executable(outlab04 main.cxx)
add_custom_command(TARGET outlab04
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab04> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(outlab04 PRIVATE methods ne591_compiler_flags fmt::fmt argparse)

set_property(TARGET outlab04 PROPERTY OUTPUT_NAME shumilov_outlab04)

install(TARGETS outlab04 DESTINATION bin OPTIONAL)