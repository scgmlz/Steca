## parametrization (version, platform-specific paths)

set(qt5_version 5.11.1)
set(_qt5_install_prefix /usr)
set(_qt5_include_prefix ${_qt5_install_prefix}/include/x86_64-linux-gnu/qt5)
set(lib_path ${_qt5_install_prefix}/lib/x86_64-linux-gnu)

## set paths

set(_qt5Core_install_prefix ${_qt5_install_prefix}) # also used by included Qt5Core*.cmake

set(qt5_components Core Gui)
foreach(comp ${qt5_components})
    set(${comp}_path "${lib_path}/libQt5${comp}.so.${qt5_version}")
    set(Qt5${comp}_INCLUDE_DIRS
        ${_qt5_include_prefix}
        ${_qt5_include_prefix}/Qt${comp})
endforeach()

list(APPEND Qt5Gui_INCLUDE_DIRS ${_qt5_include_prefix}/QtCore)

## check file existence

macro(check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "FindStaticQt5 cannot find file \"${file}\".")
    endif()
endmacro()
macro(_qt5_Core_check_file_exists file) # used by included Qt5Core*.cmake
    check_file_exists(${file})
endmacro()

foreach(comp ${qt5_components})
    check_file_exists(${${comp}_path})
    message(STATUS "StaticQt5: require lib ${${comp}_path}")
    foreach(_dir ${Qt5${comp}_INCLUDE_DIRS})
        message(STATUS "StaticQt5: require include dir ${_dir}")
        check_file_exists(${_dir})
    endforeach()
endforeach()

## set target properties of Qt5::Core etc

foreach(comp ${qt5_components})
    set(Qt5${comp}_LIBRARIES Qt5::${comp})
    add_library(Qt5::${comp} SHARED IMPORTED)
    set_target_properties(Qt5::${comp} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES   "${Qt5${comp}_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES        ""
        IMPORTED_LOCATION_RELEASE       ${${comp}_path}
        )
    set_property(TARGET Qt5::${comp} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
endforeach()

## invoke some more CMake files

set(Qt5ConfigDir "/usr/lib/x86_64-linux-gnu/cmake")
message(STATUS "StaticQt5: run code from ${Qt5ConfigDir}/*/")
foreach(comp ${qt5_components})
    include("${Qt5ConfigDir}/Qt5${comp}/Qt5${comp}ConfigExtras.cmake")
endforeach()
include("${Qt5ConfigDir}/Qt5Core/Qt5CoreMacros.cmake")
