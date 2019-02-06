if(WIN32)
    add_executable(Steca WIN32 steca.cpp resources.qrc resources.rc)
else()
    add_executable(Steca steca.cpp resources.qrc)
endif()

target_include_directories(Steca
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty)

target_link_libraries(Steca PRIVATE gui)

if(WIN32)
    include(PackIFW)
endif()
