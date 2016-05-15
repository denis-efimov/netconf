cmake_minimum_required(VERSION 2.6)
project(tests)

#message("PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR}/.)
aux_source_directory(${PROJECT_SOURCE_DIR} TESTS_SOURCES)
add_executable( runTests ${TESTS_SOURCES} )
target_link_libraries(runTests gtest gtest_main pthread)
add_test (NAME allTests COMMAND runTests)
add_custom_command(TARGET runTests
                   POST_BUILD
                   COMMAND make ARGS test)
