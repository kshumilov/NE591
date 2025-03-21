add_executable(inlab09 src/inlab09.cpp)
target_include_directories(inlab09 PRIVATE ./include)
add_custom_command(TARGET inlab09
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab09> ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(inlab09 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET inlab09 PROPERTY OUTPUT_NAME shumilov_inlab09)

install(TARGETS inlab09 DESTINATION bin OPTIONAL)