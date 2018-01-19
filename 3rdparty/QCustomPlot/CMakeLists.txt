set(lib steca_QCP)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
)

file (GLOB files *.h *.cpp)

add_library (${lib} ${files})

target_link_libraries (${lib} Qt5::Gui Qt5::Widgets)
