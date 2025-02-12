add_executable(outlab05 main.cxx)
add_custom_command(TARGET outlab05
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab05> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(outlab05 PRIVATE methods ne591_compiler_flags fmt::fmt argparse)

set_property(TARGET outlab05 PROPERTY OUTPUT_NAME shumilov_outlab05)

install(TARGETS outlab05 DESTINATION bin OPTIONAL)