include_directories (
  ${CMAKE_HOME_DIRECTORY}
  ${CMAKE_HOME_DIRECTORY}/3rdparty
)

if(WIN32)
  add_executable (Steca WIN32 steca.cpp resources.qrc)
else()
  add_executable (Steca steca.cpp resources.qrc)
endif()

target_link_libraries (Steca gui)
