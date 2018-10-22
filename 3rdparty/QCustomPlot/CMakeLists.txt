set(lib steca_QCP)

file(GLOB files *.h *.cpp)

add_library(${lib} ${files})

target_include_directories(${lib} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${lib} PRIVATE Qt5::Gui Qt5::Widgets)
