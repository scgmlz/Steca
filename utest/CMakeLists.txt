# To run the tests on Linux, use 'make test' or 'ctest'.
# To select which tests to run, use 'ctest -R regex'.
# To see output from the individual tests, use 'ctest -V'.
# For more options, run 'ctest --help'.

include(cmake/utils.cmake) # defines cxx_executable, cxx_test

set(inc_dirs
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
    ../ThirdParty
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR} # for testdata.h
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
    target_include_directories(${test_name} PRIVATE ${inc_dirs})
    target_link_libraries(${test_name} PRIVATE gtest_main core)
endforeach(test_src)

foreach(test_src ${gui_test_sources})
    # remove directory path and suffix, to retain just the name of the test
    string(REGEX REPLACE ".*/" "" test_name "${test_src}")
    string(REGEX REPLACE "\\.cpp$" "" test_name "${test_name}")
    cxx_test(${test_name} "")
    target_include_directories(${test_name} PRIVATE ${inc_dirs})
    target_link_libraries(${test_name} PRIVATE gtest_main core gui)
endforeach(test_src)
