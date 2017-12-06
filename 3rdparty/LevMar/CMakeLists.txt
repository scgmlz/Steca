get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib # LevMar is patched for message handling
)

file (GLOB_RECURSE files *.h *.hpp *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib} Qt5::Core)
