# Steca Windows packaging #


# Name of the package
set(CPACK_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-Installer")

# Enable IFW generator
set(CPACK_GENERATOR IFW)

# Default Directories for installation and Start Menu
set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY "A Steca Istallation with IFW")
set(CPACK_IFW_TARGET_DIRECTORY "@HomeDir@/${CMAKE_PROJECT_NAME}")
set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/main/icons/bee.ico")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/main/icons/bee.png")

# If not set, all components will be merged in the group
set(CPACK_COMPONENTS_GROUPING IGNORE)

# The parent group
set(CPACK_IFW_PACKAGE_GROUP Steca)

include(CPack)
include(CPackIFW)
include(CPackWindowsAddExtraLibs)
# 1. add/configure component group
# 2. install component
# 3. add/configure component
cpack_add_component_group(Steca EXPANDED)
cpack_ifw_configure_component_Group(Steca NAME fzj.jcns.scg.steca)
install(TARGETS Steca
  DESTINATION bin
  COMPONENT Steca.exe)
windeployqt("Steca" "bin")
cpack_add_component(Steca.exe ENABLED GROUP Steca)
cpack_ifw_configure_component(Steca.exe NAME fzj.jcns.scg.steca.exe SCRIPT ${CMAKE_SOURCE_DIR}/StecaInstallScript.qs)
