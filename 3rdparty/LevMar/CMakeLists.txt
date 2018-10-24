file(GLOB_RECURSE files *.h *.hpp *.cpp)
add_library(LevMar ${files})

target_include_directories(LevMar
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/core # LevMar is patched for message handling
)

if( CMAKE_COMPILER_IS_GNUCC )
    target_compile_options(LevMar PRIVATE "-Wno-misleading-indentation")
endif()

target_link_libraries(LevMar Qt5::Core)
