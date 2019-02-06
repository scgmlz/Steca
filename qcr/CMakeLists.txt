set(lib qcr)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(${lib}
    PUBLIC
    Qt5::Widgets # depends on Qt5::Gui which depends on Qt5::Core
    )
