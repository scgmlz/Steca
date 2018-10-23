## set file locations

set(_qt5Core_install_prefix "/usr") # also used by included Qt5Core*.cmake
set(_qt5Gui_install_prefix "/usr") # also used by included Qt5Core*.cmake
set(lib_path "${_qt5Core_install_prefix}/lib/x86_64-linux-gnu/libQt5Core.so.5.11.1")
set(Qt5Core_INCLUDE_DIRS
    ${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/
    ${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/QtCore)

## check file existence

macro(check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "FindStaticQt5 cannot find file \"${file}\".")
    endif()
endmacro()
macro(_qt5_Core_check_file_exists file) # used by included Qt5Core*.cmake
    check_file_exists(${file})
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



set(Qt5Gui_LIBRARIES Qt5::Gui)

macro(_populate_Gui_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::Gui APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5Gui_install_prefix}/lib/x86_64-linux-gnu/${LIB_LOCATION}")
    check_file_exists(${imported_location})
    set_target_properties(Qt5::Gui PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt5Gui_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        "IMPORTED_SONAME_${Configuration}" "libQt5Gui.so.5"
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt5Gui_LIB_DEPENDENCIES}"
    )

endmacro()

set(_Qt5Gui_OWN_INCLUDE_DIRS "${_qt5Gui_install_prefix}/include/x86_64-linux-gnu/qt5/" "${_qt5Gui_install_prefix}/include/x86_64-linux-gnu/qt5/QtGui")

foreach(_dir ${_Qt5Gui_OWN_INCLUDE_DIRS})
    check_file_exists(${_dir})
endforeach()

set(Qt5Gui_INCLUDE_DIRS ${_Qt5Gui_OWN_INCLUDE_DIRS})

set(Qt5Gui_DEFINITIONS -DQT_GUI_LIB)
set(Qt5Gui_COMPILE_DEFINITIONS QT_GUI_LIB)
set(_Qt5Gui_MODULE_DEPENDENCIES "Core")


foreach(_module_dep ${_Qt5Gui_MODULE_DEPENDENCIES})
    if (NOT Qt5${_module_dep}_FOUND)
        find_package(Qt5${_module_dep}
            5.11.1 ${_Qt5Gui_FIND_VERSION_EXACT}
            ${_Qt5Gui_DEPENDENCIES_FIND_QUIET}
            ${_Qt5Gui_FIND_DEPENDENCIES_REQUIRED}
            PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
    endif()

    if (NOT Qt5${_module_dep}_FOUND)
        set(Qt5Gui_FOUND False)
        return()
    endif()

    list(APPEND Qt5Gui_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
    list(APPEND Qt5Gui_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
    list(APPEND Qt5Gui_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
    list(APPEND Qt5Gui_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
endforeach()
list(REMOVE_DUPLICATES Qt5Gui_INCLUDE_DIRS)
list(REMOVE_DUPLICATES Qt5Gui_DEFINITIONS)
list(REMOVE_DUPLICATES Qt5Gui_COMPILE_DEFINITIONS)

set(_Qt5Gui_LIB_DEPENDENCIES "Qt5::Core")


add_library(Qt5::Gui SHARED IMPORTED)

set_property(TARGET Qt5::Gui PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${_Qt5Gui_OWN_INCLUDE_DIRS})
set_property(TARGET Qt5::Gui PROPERTY
    INTERFACE_COMPILE_DEFINITIONS QT_GUI_LIB)

_populate_Gui_target_properties(RELEASE "libQt5Gui.so.5.11.1" "" )


file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt5Gui_*Plugin.cmake")

macro(_populate_Gui_plugin_properties Plugin Configuration PLUGIN_LOCATION)
    set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5Gui_install_prefix}/lib/x86_64-linux-gnu/qt5/plugins/${PLUGIN_LOCATION}")
    check_file_exists(${imported_location})
    set_target_properties(Qt5::${Plugin} PROPERTIES
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
endmacro()

if (pluginTargets)
    foreach(pluginTarget ${pluginTargets})
        include(${pluginTarget})
    endforeach()
endif()


include("${CMAKE_CURRENT_LIST_DIR}/Qt5GuiConfigExtras.cmake")


check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt5GuiConfigVersion.cmake")
