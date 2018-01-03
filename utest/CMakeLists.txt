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
    ../core
    ${CMAKE_BINARY_DIR}
    )

link_directories(
    ${gtest_BINARY_DIR}/src
    ../ThirdParty
    ../core
    )

set(test_link_libs
    gtest_main
    core
    # for test010_io, progress bar:
    Qt5::Gui
    Qt5::Widgets
    )

# We glob test sources, though this is often considered evil.
# Just touch CMakeLists.txt each time a test is added or withdrawn.

file(GLOB test_sources "test[0-9]*.cpp")
list(SORT test_sources)

foreach(test_src ${test_sources})
    # remove directory path and suffix, to retain just the name of the test
    string(REGEX REPLACE ".*/" "" test_name "${test_src}")
    string(REGEX REPLACE "\\.cpp$" "" test_name "${test_name}")
    cxx_test(${test_name} "${test_link_libs}")
endforeach(test_src)
