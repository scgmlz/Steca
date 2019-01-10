cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
if(POLICY CMP0048)
   cmake_policy(SET CMP0048 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

option(CERF_CPP "Build libcerf as native C++" OFF)
option(LIB_MAN "Build libcerf man pages" ON)
option(LIB_INSTALL "Install libcerf" ON)
option(PEDANTIC "Compile with pedantic warnings" ON)

if(${CERF_CPP})
    project(cerfcpp CXX)
    # add_compile_definitions(CERF_AS_CPP) # requires CMake 3.13, therefore more explicitly:
    if(MSVC)
        add_compile_options(/DCERF_AS_CPP)
    else()
        add_compile_options(-DCERF_AS_CPP)
    endif()
    set(CMAKE_CXX_STANDARD 14)
else()
    project(cerf C)
    set(CMAKE_C_STANDARD 99)
endif()

set(CERF_SOVERSION                 1) # API version
set(CERF_VERSION ${CERF_SOVERSION}.11) # minor version

if(MSVC)
else()
    add_compile_options(-O2)
    if(PEDANTIC)
        add_compile_options(-pedantic -Wall)
    endif()
endif()

include(CTest)

set(destination ${CMAKE_INSTALL_PREFIX})

configure_file("libcerf.pc.in" "libcerf.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libcerf.pc"
    DESTINATION "${destination}/lib/pkgconfig/")

add_subdirectory(lib)
add_subdirectory(run)
add_subdirectory(test)
if (${LIB_MAN})
    add_subdirectory(man)
endif()
