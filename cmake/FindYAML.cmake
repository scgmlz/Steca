# CMake module to search for the C library libyaml
#
# If the library is found, then LIBYAML_FOUND is set to TRUE,
# and following variables are set:
#    LIBYAML_INCLUDE_DIR
#    LIBYAML_LIBRARIES

find_path(LIBYAML_INCLUDE_DIR NAMES yaml.h)
find_library(LIBYAML_LIBRARIES NAMES yaml libyaml)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yaml DEFAULT_MSG LIBYAML_LIBRARIES LIBYAML_INCLUDE_DIR)
mark_as_advanced(LIBYAML_INCLUDE_DIR LIBYAML_LIBRARIES)
