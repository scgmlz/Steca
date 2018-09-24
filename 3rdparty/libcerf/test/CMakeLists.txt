include_directories(${CMAKE_CURRENT_BINARY_DIR} ${cerf_SOURCE_DIR}/lib)

set(tests
    runvoigt
    run_w_of_z
    test1
    )

foreach(test ${tests})
    add_executable(${test} ${test}.c)
    target_link_libraries(${test} ${cerf_LIBRARY})
endforeach()
