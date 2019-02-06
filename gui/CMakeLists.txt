set(lib gui)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

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
