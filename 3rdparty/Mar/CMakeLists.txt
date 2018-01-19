set(lib mar_loader)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib # for message callback
)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-w") # legacy code with many warnings
endif ()

file (GLOB files *.h *.hpp *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib} Qt5::Core)
