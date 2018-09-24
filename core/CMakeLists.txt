get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${LIBYAML_INCLUDE_DIR}
  ${CMAKE_HOME_DIRECTORY}
  ${CMAKE_HOME_DIRECTORY}/3rdparty
)

file (GLOB_RECURSE files *.h *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
    qcr
    LevMar
    caress_loader
    mar_loader
    cerf
    ${LIBYAML_LIBRARY}
)
