get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/3rdparty/LevMar
)

file (GLOB_RECURSE files *.h *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
  Qt5::Core
)
