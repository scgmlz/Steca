get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE files *.cpp)
add_library(${lib} ${files})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
    )

target_link_libraries(${lib}
    PUBLIC
    core
    PRIVATE
    Qt5::Network
    steca_QCP
    )
