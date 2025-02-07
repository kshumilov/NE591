add_executable(inlab05 main.cxx)
add_custom_command(TARGET inlab05
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab05> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(inlab05 PRIVATE methods ne591_compiler_flags fmt::fmt argparse)

set_property(TARGET inlab05 PROPERTY OUTPUT_NAME shumilov_inlab05)

install(TARGETS inlab05 DESTINATION bin OPTIONAL)