set(library_name cerf)
set(${library_name}_LIBRARY ${library_name} PARENT_SCOPE)
set(${library_name}_LIBRARY_TYPE SHARED)

set(src_files erfcx.c err_fcts.c im_w_of_x.c w_of_z.c)
set(inc_files cerf.h defs.h)

add_library(${library_name} SHARED ${src_files})

set_target_properties(
    ${library_name} PROPERTIES
    OUTPUT_NAME ${library_name}
    VERSION ${cerf_VERSION}
    SOVERSION ${cerf_SOVERSION})

target_link_libraries(${library_name} m)

install(
    TARGETS ${library_name} LIBRARY
    DESTINATION ${destination}/lib
    COMPONENT Libraries)
install(
    FILES ${inc_files}
    DESTINATION ${destination}/include
    COMPONENT Headers)
