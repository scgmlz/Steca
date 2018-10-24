message(STATUS "Setup Qt5 library dependences according to ${qt5_settings}")

## set paths

set(qt5_components Core Gui Widgets Network)
foreach(comp ${qt5_components})
    set(qt5_${comp}_location "${qt5_lib_prefix}Qt5${comp}.${qt5_lib_postfix}")
    set(qt5_${comp}_inc_dirs
        ${qt5_include_prefix}
        ${qt5_include_prefix}/Qt${comp})
endforeach()

list(APPEND qt5_Gui_inc_dirs     ${qt5_include_prefix}/QtCore)
list(APPEND qt5_Widgets_inc_dirs ${qt5_include_prefix}/QtCore)
list(APPEND qt5_Widgets_inc_dirs ${qt5_include_prefix}/QtGui)
list(APPEND qt5_Network_inc_dirs ${qt5_include_prefix}/QtCore)

set(qt5_Gui_link_libs     "Qt5::Core")
set(qt5_Widgets_link_libs "Qt5::Gui")
set(qt5_Network_link_libs "Qt5::Core")

## check file existence

macro(check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "FindStaticQt5 cannot find file \"${file}\".")
    endif()
endmacro()

foreach(comp ${qt5_components})
    message(STATUS "StaticQt5: require lib ${qt5_${comp}_location}")
    check_file_exists(${qt5_${comp}_location})
    foreach(_dir ${Qt5${comp}_INCLUDE_DIRS})
        message(STATUS "StaticQt5: require include dir ${_dir}")
        check_file_exists(${_dir})
    endforeach()
endforeach()

## set target properties of Qt5::Core etc

foreach(comp ${qt5_components})
    # set(Qt5${comp}_LIBRARIES Qt5::${comp})
    add_library(Qt5::${comp} ${qt5_link_mode} IMPORTED)
    set_target_properties(Qt5::${comp} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES   "${qt5_${comp}_inc_dirs}"
        INTERFACE_LINK_LIBRARIES        "${qt5_${comp}_link_libs}"
        IMPORTED_CONFIGURATIONS         RELEASE
        IMPORTED_LOCATION_RELEASE       ${qt5_${comp}_location}
        )
    message(STATUS "StaticQt5: target Qt5::${comp} has location ${qt5_${comp}_location}")
endforeach()

## invoke some more CMake files

message(STATUS "StaticQt5: run code from ${qt5_config_dir}/*/")
foreach(comp Core Gui Widgets)
    macro(_qt5_${comp}_check_file_exists file)
        check_file_exists(${file})
    endmacro()
    set(_qt5${comp}_install_prefix ${qt5_install_prefix})
    include("${qt5_config_dir}/Qt5${comp}/Qt5${comp}ConfigExtras.cmake")
endforeach()

include("${qt5_config_dir}/Qt5Core/Qt5CoreMacros.cmake")
include("${qt5_config_dir}/Qt5Widgets/Qt5WidgetsMacros.cmake")
