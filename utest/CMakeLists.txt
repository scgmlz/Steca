# To run the tests on Linux, use 'make test' or 'ctest'.
# To select which tests to run, use 'ctest -R regex'.
# To see output from the individual tests, use 'ctest -V'.
# For more options, run 'ctest --help'.

include(cmake/utils.cmake) # defines cxx_executable, cxx_test

enable_testing()

include_directories(
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
    ../ThirdParty
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

link_directories(
    ${gtest_BINARY_DIR}/src
    ../ThirdParty
    ../core
    )

set(core_test_link_libs
    gtest_main
    core
    )

set(gui_test_link_libs
    gtest_main
    core
    gui
    )

# We glob test sources, though this is often considered evil.
# Just touch CMakeLists.txt each time a test is added or withdrawn.

file(GLOB core_test_sources "test0*.cpp")
file(GLOB gui_test_sources  "test1*.cpp")
list(SORT core_test_sources)
list(SORT gui_test_sources)

# TODO: avoid the following duplication of code

foreach(test_src ${core_test_sources})
    # remove directory path and suffix, to retain just the name of the test
    string(REGEX REPLACE ".*/" "" test_name "${test_src}")
    string(REGEX REPLACE "\\.cpp$" "" test_name "${test_name}")
    cxx_test(${test_name} "")
    target_link_libraries(${test_name} "${core_test_link_libs}")
endforeach(test_src)

foreach(test_src ${gui_test_sources})
    # remove directory path and suffix, to retain just the name of the test
    string(REGEX REPLACE ".*/" "" test_name "${test_src}")
    string(REGEX REPLACE "\\.cpp$" "" test_name "${test_name}")
    cxx_test(${test_name} "")
    target_link_libraries(${test_name} "${gui_test_link_libs}")
endforeach(test_src)
