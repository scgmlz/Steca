## set file locations

set(_qt5Core_install_prefix "/usr") # also used by included Qt5Core*.cmake
set(lib_path "${_qt5Core_install_prefix}/lib/x86_64-linux-gnu/libQt5Core.so.5.11.1")
set(Qt5Core_INCLUDE_DIRS
    ${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/
    ${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/QtCore)

## check file existence

macro(_qt5_Core_check_file_exists file) # also used by included Qt5Core*.cmake
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "FindStaticQt5 cannot find file \"${file}\".")
    endif()
endmacro()

_qt5_Core_check_file_exists(${lib_path})
foreach(_dir ${Qt5Core_INCLUDE_DIRS})
    _qt5_Core_check_file_exists(${_dir})
    message(STATUS "StaticQt5: found include dir ${_dir}")
endforeach()

## set target properties of Qt5::Core

set(Qt5Core_LIBRARIES Qt5::Core)
add_library(Qt5::Core SHARED IMPORTED)
set_target_properties(Qt5::Core PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES   "${Qt5Core_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES        ""
    IMPORTED_LOCATION_RELEASE       ${lib_path}
    )
set_property(TARGET Qt5::Core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

## standard settings from Qt5 supplied .cmake files

set(Qt5CoreConfigDir "/usr/lib/x86_64-linux-gnu/cmake/Qt5Core")
message(STATUS "StaticQt5: run code from ${Qt5CoreConfigDir}")
include("${Qt5CoreConfigDir}/Qt5CoreConfigExtras.cmake")
include("${Qt5CoreConfigDir}/Qt5CoreMacros.cmake")
