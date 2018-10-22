
if(WIN32)
    set(WIN_RESOURCE_FILE resources.rc)
    add_executable (Steca WIN32 steca.cpp resources.qrc ${WIN_RESOURCE_FILE})
else()
    add_executable (Steca steca.cpp resources.qrc)
endif()

target_include_directories (Steca
    PRIVATE
    ${CMAKE_HOME_DIRECTORY}
    ${CMAKE_HOME_DIRECTORY}/3rdparty)

target_link_libraries (Steca gui)
if(WIN32)
    include(CPackWindows)
endif()
