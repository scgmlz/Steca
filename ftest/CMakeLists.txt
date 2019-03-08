file(GLOB test_scripts "*.cmd")

foreach(test_src ${test_scripts})
    # remove directory path and suffix, to retain just the name of the test
    string(REGEX REPLACE ".*/" "" test_name "${test_src}")
    string(REGEX REPLACE "\\.cmd$" "" test_name "${test_name}")

    # copy and configure the test script
    set(test_exe "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.cmd")
    configure_file(${test_src} ${test_exe})
    # append "quit" to prevent Steca from dangling (waiting for more input)
    file(APPEND ${test_exe} "quit")

    # create directory where Steca writes its output files
    set(out_dir "out")
    file(REMOVE_RECURSE ${out_dir})
    file(MAKE_DIRECTORY ${out_dir})

    # run Steca
    add_test(
        NAME ${test_name}
        COMMAND ${CMAKE_BINARY_DIR}/main/Steca ${test_exe})

endforeach()
