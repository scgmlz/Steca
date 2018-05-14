include_directories (
  ${CMAKE_HOME_DIRECTORY}
  ${CMAKE_HOME_DIRECTORY}/3rdparty)

if(WIN32)
  set(WIN_RESOURCE_FILE resources.rc)
  add_executable (Steca WIN32 steca.cpp resources.qrc ${WIN_RESOURCE_FILE})
else()
  add_executable (Steca steca.cpp resources.qrc)
endif()

target_link_libraries (Steca gui)

##### CPACK

set(CPACK_PACKAGE_NAME "Steca")
set(CPACK_GENERATOR "IFW")
set(STECA_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "Steca: Stress and texture calculator for neutron and x-ray materials diffraction.")
set(CPACK_PACKAGE_VENDOR "Forschungszentrum Juelich GmbH. Scientific Computing Group at MLZ Garching.")
set(CPACK_PACKAGE_VERSION ${STECA_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}-${STECA_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_IFW_PACKAGE_GROUP Steca)

include(CPack)
include(CPackIFW)

cpack_add_component_group(Steca EXPANDED)
cpack_ifw_configure_component_group(Steca NAME org.foo.installer.g0)
install(TARGETS Steca
  DESTINATION Steca
  COMPONENT Steca)
cpack_add_component(Steca
  DISABLED
  GROUP StecaA)
cpack_ifw_configure_component(Steca
  NAME org.foo.installer.Steca)
############

#Add some groups
foreach(_g g1 g2 g3)
  cpack_add_component_group(${_g} PARENT_GROUP Steca)
  cpack_ifw_configure_component_group(${_g} NAME org.foo.installer.${_g})

  foreach(_f a b c d)
    # Add some components
    file(WRITE "${CMAKE_BINARY_DIR}/${_g}/${_f}.txt" "${_f}.txt contents\n")
    install(FILES "${CMAKE_BINARY_DIR}/${_g}/${_f}.txt"
      DESTINATION ${_g}
      COMPONENT ${_g}_${_f})
    cpack_add_component(${_g}_${_f}
      DISABLED
      GROUP ${_g})
    cpack_ifw_configure_component(${_g}_${_f} 
      NAME org.foo.installer.${_g}.${_f}
      )
  endforeach()

endforeach()
############

macro(qt5_copy_dll APP DLL)
  # find the release *.dll file
  get_target_property(Qt5_${DLL}Location Qt5::${DLL} LOCATION)
  # find the debug *d.dll file
  get_target_property(Qt5_${DLL}LocationDebug Qt5::${DLL} IMPORTED_LOCATION_DEBUG)

  add_custom_command(TARGET ${APP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<CONFIG:Debug>:${Qt5_${DLL}LocationDebug}>
    $<$<NOT:$<CONFIG:Debug>>:${Qt5_${DLL}Location}>
    $<TARGET_FILE_DIR:${APP}>)
endmacro()

qt5_copy_dll(Steca Core)
qt5_copy_dll(Steca Widgets)
qt5_copy_dll(Steca Network)
