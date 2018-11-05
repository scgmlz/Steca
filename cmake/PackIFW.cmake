# Configure a Steca installer.
# Currently only tested and used for Windows.
# Relies on code adapted from "Using windeployqt with CPack",
#   https://hk.saowen.com/a/d1cf90fcfea6d511629fd5a6c8113808721a7f19656677e8a5fab370a8d35cd4

set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-Installer")
set(CPACK_PACKAGE_VENDOR "Forschungszentrum Juelich GmbH")
# the next three lines are required for CMake <3.12
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_VERBATIM_VARIABLES TRUE) # urgent advise from Scott book

set(CPACK_GENERATOR IFW)

set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY "") # empty => default is install to top-level (?)
set(CPACK_IFW_TARGET_DIRECTORY "@HomeDir@/${CMAKE_PROJECT_NAME}")
set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/main/icons/Steca.ico")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/main/icons/Steca.png")

set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_IFW_PACKAGE_GROUP Steca)

include(CPack)
include(CPackIFW)

# Search for compiler-provided system runtime libraries and add install rules for them.
# Options must be given before the include.
set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE) # install Windows Universal CRT libs for app-local deployment
include(InstallRequiredSystemLibraries)

# Find windeployqt.exe (a Qt tool, should be in QTDIR/bin, along with qmake.exe, uic.exe etc)
if(NOT DEFINED ENV{QTDIR})
    message(FATAL_ERROR "QTDIR not defined")
endif()
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS $ENV{QTDIR}/bin)
mark_as_advanced(WINDEPLOYQT_EXECUTABLE)
message(STATUS "found windeployqt tool at ${WINDEPLOYQT_EXECUTABLE}")
if(NOT WINDEPLOYQT_EXECUTABLE)
    message(FATAL_ERROR "windeployqt not found")
endif()

cpack_add_component_group(Steca EXPANDED)
cpack_ifw_configure_component_group(Steca NAME fzj.jcns.scg.steca)

install(TARGETS Steca DESTINATION bin COMPONENT Steca.exe)

# Add commands that copy the Qt runtime to the target's output directory
# after build and install the Qt runtime to the specified directory

# Run windeployqt immediately after build
add_custom_command(TARGET Steca POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
    --verbose 0
    --no-compiler-runtime
    \"$<TARGET_FILE:Steca>\"
    )

# install(CODE ...) doesn't support generator expressions, but
# file(GENERATE ...) does - store the path in a file
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Steca_$<CONFIG>"
    CONTENT "$<TARGET_FILE:Steca>"
    CONDITION "$<CONFIG:Release>"
    )

# Before installation, run a series of commands that copy each of the Qt
# runtime files to the appropriate directory for installation
install(CODE
    "
    file(READ \"${CMAKE_CURRENT_BINARY_DIR}/Steca_Release\" _file)
    execute_process(
      COMMAND \"${CMAKE_COMMAND}\" -E
      env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
      --dry-run
      --no-compiler-runtime
      --list mapping
      \${_file}
      OUTPUT_VARIABLE _output
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    separate_arguments(_files WINDOWS_COMMAND \${_output})
    while(_files)
      list(GET _files 0 _src)
      list(GET _files 1 _dest)
      execute_process(
        COMMAND \"${CMAKE_COMMAND}\" -E
        copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/bin/\${_dest}\"
        )
      list(REMOVE_AT _files 0 1)
    endwhile()
    "
    COMPONENT "QtLibs"
    )

# windeployqt doesn't work correctly with the system runtime libraries,
# so we fall back to one of CMake's own modules for copying them over
foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
    get_filename_component(filename "${lib}" NAME)
    add_custom_command(TARGET Steca POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        copy_if_different "${lib}" \"$<TARGET_FILE_DIR:Steca>\"
        )
endforeach()

cpack_add_component(Steca.exe ENABLED GROUP Steca)
cpack_ifw_configure_component(Steca.exe
    NAME fzj.jcns.scg.steca.exe SCRIPT ${CMAKE_SOURCE_DIR}/StecaInstallScript.qs)
