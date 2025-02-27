add_executable(lab07 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(lab07 PRIVATE MPI::MPI_CXX
                                    fmt::fmt)

set_property(TARGET lab07 PROPERTY OUTPUT_NAME shumilov_inlab07)

add_custom_command(TARGET lab07
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lab07> ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS lab07 DESTINATION bin OPTIONAL)


add_executable(outlab07 ${CMAKE_CURRENT_SOURCE_DIR}/src/outlab07.cpp)

target_link_libraries(outlab07 PRIVATE
                                    ne591_compiler_flags
                                    MPI::MPI_CXX
                                    fmt::fmt
                                    argparse)

set_property(TARGET outlab07 PROPERTY OUTPUT_NAME shumilov_outlab07)

add_custom_command(TARGET outlab07
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab07> ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS outlab07 DESTINATION bin OPTIONAL)
