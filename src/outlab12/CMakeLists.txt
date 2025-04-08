add_executable(outlab12 src/outlab12.cpp)
target_include_directories(outlab12 PRIVATE ./include)
add_custom_command(TARGET outlab12
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab12> ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(outlab12 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET outlab12 PROPERTY OUTPUT_NAME shumilov_outlab12)

install(TARGETS outlab12 DESTINATION bin OPTIONAL)
