# To run the tests on Linux, use 'make test' or 'ctest'.
# To select which tests to run, use 'ctest -R regex'.
# To see output from the individual tests, use 'ctest -V'.
# For more options, run 'ctest --help'.

include(cmake/utils.cmake) # defines cxx_executable, cxx_test
function(my_cxx_test name source libs)
    cxx_test_with_flags("${name}" "${cxx_default}" "${libs}" "${source}" ${ARGN})
endfunction()

set(inc_dirs
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
    ${CMAKE_HOME_DIRECTORY}/ThirdParty
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR} # for testdata.h
    )

add_subdirectory(qcr/local)
add_subdirectory(core/local)
add_subdirectory(core/linked)
