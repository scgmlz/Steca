# core/CMakeLists.txt

include_directories (
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/LevMar
  ${CMAKE_HOME_DIRECTORY}/lib
)

file (GLOB header_files
  "*.h" "*/*.h" "*/*/*.h"
)

file (GLOB source_files
  "*.cpp" "*/*.cpp" "*/*/*.cpp"
)

add_library (StecaCore
  ${source_files}
  ${header_files}
)

target_link_libraries (StecaCore
  Qt5::Core
  Qt5::Gui # TODO for QImage in tiffImage
)

#eof
