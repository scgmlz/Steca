if(WIN32)
    add_executable(numdiff WIN32 numdiff.cpp)
else()
    add_executable(numdiff numdiff.cpp)
endif()

target_include_directories(numdiff
    PRIVATE
    ${CMAKE_SOURCE_DIR})

if(WIN32)
    include(PackIFW)
endif()
