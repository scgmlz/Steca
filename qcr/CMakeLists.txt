get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE files *.h *.cpp)
add_library(${lib} ${files})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
    )

target_link_libraries(${lib}
    PUBLIC
    Qt5::Widgets # depends on Qt5::Gui which depends on Qt5::Core
    )
