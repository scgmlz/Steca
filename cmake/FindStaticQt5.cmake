## parametrization (version, platform-specific paths)

set(qt5_version 5.11.1)
set(_qt5_install_prefix /usr)
set(_qt5_include_prefix ${_qt5_install_prefix}/include/x86_64-linux-gnu/qt5)
set(lib_path ${_qt5_install_prefix}/lib/x86_64-linux-gnu)

## set paths

set(qt5_components Core Gui Widgets Network)
foreach(comp ${qt5_components})
    set(${comp}_path "${lib_path}/libQt5${comp}.so.${qt5_version}")
    set(Qt5${comp}_INCLUDE_DIRS
        ${_qt5_include_prefix}
        ${_qt5_include_prefix}/Qt${comp})
endforeach()

list(APPEND Qt5Gui_INCLUDE_DIRS ${_qt5_include_prefix}/QtCore)
list(APPEND Qt5Widgets_INCLUDE_DIRS ${_qt5_include_prefix}/QtCore)
list(APPEND Qt5Widgets_INCLUDE_DIRS ${_qt5_include_prefix}/QtGui)
list(APPEND Qt5Network_INCLUDE_DIRS ${_qt5_include_prefix}/QtCore)

set(Gui_LIB_DEPENDENCIES     "Qt5::Core")
set(Widgets_LIB_DEPENDENCIES "Qt5::Gui;Qt5::Core")
set(Network_LIB_DEPENDENCIES "Qt5::Core")

## check file existence

macro(check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "FindStaticQt5 cannot find file \"${file}\".")
    endif()
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
        INTERFACE_LINK_LIBRARIES        "${${comp}_LIB_DEPENDENCIES}"

        IMPORTED_LOCATION_RELEASE       ${${comp}_path}
        )
    set_property(TARGET Qt5::${comp} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
endforeach()

## invoke some more CMake files

set(Qt5ConfigDir "/usr/lib/x86_64-linux-gnu/cmake")
message(STATUS "StaticQt5: run code from ${Qt5ConfigDir}/*/")
foreach(comp Core Gui Widgets)
    macro(_qt5_${comp}_check_file_exists file)
        check_file_exists(${file})
    endmacro()
    set(_qt5${comp}_install_prefix ${_qt5_install_prefix})
    include("${Qt5ConfigDir}/Qt5${comp}/Qt5${comp}ConfigExtras.cmake")
endforeach()

include("${Qt5ConfigDir}/Qt5Core/Qt5CoreMacros.cmake")
include("${Qt5ConfigDir}/Qt5Widgets/Qt5WidgetsMacros.cmake")
