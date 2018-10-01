include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set(tests
    runvoigt
    run_w_of_z
    test1
    )

foreach(test ${tests})
    if (${CERF_CPP})
        set_property(SOURCE ${test}.c PROPERTY LANGUAGE CXX)
    endif()
    add_executable(${test} ${test}.c)
    if (${CERF_CPP})
        set_property(TARGET ${test} PROPERTY LINKER_LANGUAGE CXX)
    endif()
    target_link_libraries(${test} ${cerf_LIBRARY})
endforeach()
