# LevMar

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file (GLOB_RECURSE files *.h *.hpp *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
  Qt5::Core
)

# eof
