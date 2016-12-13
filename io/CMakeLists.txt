# io

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-w") # legacy code with many warnings
endif ()

file (GLOB_RECURSE files *.h *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
  Qt5::Core
)

target_link_libraries (${lib}
  Qt5::Core
)

#eof
