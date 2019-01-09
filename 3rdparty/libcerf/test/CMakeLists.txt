set(tests
    test1
    )

foreach(test ${tests})
    if (${CERF_CPP})
        set_property(SOURCE ${test}.c PROPERTY LANGUAGE CXX)
    endif()
    add_executable(${test} ${test}.c)
    target_include_directories(${test} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    if (${CERF_CPP})
        set_property(TARGET ${test} PROPERTY LINKER_LANGUAGE CXX)
    endif()
    target_link_libraries(${test} ${cerf_LIBRARY})
    add_test(${test} ${CMAKE_CURRENT_BINARY_DIR}/${test})
endforeach()
