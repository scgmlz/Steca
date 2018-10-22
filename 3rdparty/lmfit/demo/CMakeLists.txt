set(demos
    curve1
    surface1
    nonlin1
    )

foreach(demo ${demos})
    add_executable(${demo} ${demo}.c)
    target_link_libraries(${demo} ${lmfit_LIBRARY})
    target_include_directories(${demo} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${lmfit_SOURCE_DIR}/lib)
endforeach()
