if(WIN32)
    add_executable(Steca WIN32 steca.cpp resources.qrc resources.rc)
else()
    add_executable(Steca steca.cpp resources.qrc)
endif()

target_include_directories(Steca
    PRIVATE
    ${CMAKE_HOME_DIRECTORY}
    ${CMAKE_HOME_DIRECTORY}/3rdparty)

target_link_libraries(Steca PRIVATE gui)
