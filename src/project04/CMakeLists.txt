add_executable(project04 src/project.cpp src/mpi_utils.cpp)
target_include_directories(project04 PRIVATE ./include)
add_custom_command(TARGET project04
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:project04> ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(project04 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json MPI::MPI_CXX)
set_property(TARGET project04 PROPERTY OUTPUT_NAME shumilov_project04)

install(TARGETS project04 DESTINATION bin OPTIONAL)