set(_qt5Core_install_prefix "/usr")

set(Qt5Core_LIBRARIES Qt5::Core)

# The following macro is also used by files Qt5Core*.cmake that are included below.
macro(_qt5_Core_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "FindStaticQt5 cannot find file \"${file}\".")
    endif()
endmacro()


set(_Qt5Core_OWN_INCLUDE_DIRS "${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/" "${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/QtCore")
set(Qt5Core_PRIVATE_INCLUDE_DIRS
    "${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/QtCore/5.11.1"
    "${_qt5Core_install_prefix}/include/x86_64-linux-gnu/qt5/QtCore/5.11.1/QtCore"
    )

foreach(_dir ${_Qt5Core_OWN_INCLUDE_DIRS})
    _qt5_Core_check_file_exists(${_dir})
    message(STATUS "StaticQt5: found include dir ${_dir}")
endforeach()

# Only check existence of private includes if the Private component is specified.
list(FIND Qt5Core_FIND_COMPONENTS Private _check_private)
if (NOT _check_private STREQUAL -1)
    foreach(_dir ${Qt5Core_PRIVATE_INCLUDE_DIRS})
        _qt5_Core_check_file_exists(${_dir})
    endforeach()
endif()

set(Qt5Core_INCLUDE_DIRS ${_Qt5Core_OWN_INCLUDE_DIRS})

set(Qt5Core_DEFINITIONS -DQT_CORE_LIB)
set(Qt5Core_COMPILE_DEFINITIONS QT_CORE_LIB)
set(Qt5Core_OWN_PRIVATE_INCLUDE_DIRS ${Qt5Core_PRIVATE_INCLUDE_DIRS})

set(_Qt5Core_FIND_DEPENDENCIES_REQUIRED)
if (Qt5Core_FIND_REQUIRED)
    set(_Qt5Core_FIND_DEPENDENCIES_REQUIRED REQUIRED)
endif()
set(_Qt5Core_FIND_DEPENDENCIES_QUIET)
if (Qt5Core_FIND_QUIETLY)
    set(_Qt5Core_DEPENDENCIES_FIND_QUIET QUIET)
endif()
set(_Qt5Core_FIND_VERSION_EXACT)
if (Qt5Core_FIND_VERSION_EXACT)
    set(_Qt5Core_FIND_VERSION_EXACT EXACT)
endif()

set(Qt5Core_EXECUTABLE_COMPILE_FLAGS "")

list(REMOVE_DUPLICATES Qt5Core_INCLUDE_DIRS)
list(REMOVE_DUPLICATES Qt5Core_PRIVATE_INCLUDE_DIRS)
list(REMOVE_DUPLICATES Qt5Core_DEFINITIONS)
list(REMOVE_DUPLICATES Qt5Core_COMPILE_DEFINITIONS)
list(REMOVE_DUPLICATES Qt5Core_EXECUTABLE_COMPILE_FLAGS)

set(_Qt5Core_LIB_DEPENDENCIES "")


add_library(Qt5::Core SHARED IMPORTED)

set_property(TARGET Qt5::Core PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${_Qt5Core_OWN_INCLUDE_DIRS})
set_property(TARGET Qt5::Core PROPERTY
    INTERFACE_COMPILE_DEFINITIONS QT_CORE_LIB)

set(_Qt5Core_PRIVATE_DIRS_EXIST TRUE)
foreach (_Qt5Core_PRIVATE_DIR ${Qt5Core_OWN_PRIVATE_INCLUDE_DIRS})
    if (NOT EXISTS ${_Qt5Core_PRIVATE_DIR})
        set(_Qt5Core_PRIVATE_DIRS_EXIST FALSE)
    endif()
endforeach()

if (_Qt5Core_PRIVATE_DIRS_EXIST)
    add_library(Qt5::CorePrivate INTERFACE IMPORTED)
    set_property(TARGET Qt5::CorePrivate PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Qt5Core_OWN_PRIVATE_INCLUDE_DIRS}
        )
    set(_Qt5Core_PRIVATEDEPS)
    foreach(dep ${_Qt5Core_LIB_DEPENDENCIES})
        if (TARGET ${dep}Private)
            list(APPEND _Qt5Core_PRIVATEDEPS ${dep}Private)
        endif()
    endforeach()
    set_property(TARGET Qt5::CorePrivate PROPERTY
        INTERFACE_LINK_LIBRARIES Qt5::Core ${_Qt5Core_PRIVATEDEPS}
        )
endif()

set_property(TARGET Qt5::Core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

set(imported_location "${_qt5Core_install_prefix}/lib/x86_64-linux-gnu/libQt5Core.so.5.11.1")
_qt5_Core_check_file_exists(${imported_location})
set_target_properties(Qt5::Core PROPERTIES
    "INTERFACE_LINK_LIBRARIES" "${_Qt5Core_LIB_DEPENDENCIES}"
    "IMPORTED_LOCATION_RELEASE" ${imported_location}
    "IMPORTED_SONAME_RELEASE" "libQt5Core.so.5"
    # For backward compatibility with CMake < 2.8.12
    "IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE" "${_Qt5Core_LIB_DEPENDENCIES}"
    )

set(Qt5CoreConfigDir "/usr/lib/x86_64-linux-gnu/cmake/Qt5Core")
message(STATUS "StaticQt5: run code from ${Qt5CoreConfigDir}")
include("${Qt5CoreConfigDir}/Qt5CoreConfigExtras.cmake")
include("${Qt5CoreConfigDir}/Qt5CoreMacros.cmake")
_qt5_Core_check_file_exists("${Qt5CoreConfigDir}/Qt5CoreConfigVersion.cmake")
