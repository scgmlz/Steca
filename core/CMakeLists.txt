get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE files *.h *.cpp)
add_library(${lib} ${files})

target_include_directories(${lib}
    PRIVATE
    ${LIBYAML_INCLUDE_DIR}
    ${CMAKE_HOME_DIRECTORY}
    ${CMAKE_HOME_DIRECTORY}/3rdparty
    )

target_link_libraries(${lib}
    PUBLIC
    qcr
    PRIVATE
    LevMar
    caress_loader
    mar_loader
    ${Cerf_LIBRARY}
    ${LMFit_LIBRARY}
    ${yaml_LIBRARY}
    )
