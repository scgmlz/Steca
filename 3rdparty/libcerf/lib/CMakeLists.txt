set(library_name cerf)
if (${CERF_CPP})
    set(library_name cerfcpp)
endif()

set(cerf_LIBRARY ${library_name} PARENT_SCOPE)

set(src_files erfcx.c err_fcts.c im_w_of_x.c w_of_z.c)
set(inc_files cerf.h defs.h)

if (${CERF_CPP})
    foreach (src in ${src_files})
        set_property(SOURCE ${src} PROPERTY LANGUAGE CXX)
    endforeach()
endif()

add_library(${library_name} SHARED ${src_files})

set_target_properties(
    ${library_name} PROPERTIES
    OUTPUT_NAME ${library_name}
    VERSION ${CERF_VERSION}
    SOVERSION ${CERF_SOVERSION})
if (${CERF_CPP})
    set_property(TARGET ${library_name} PROPERTY LINKER_LANGUAGE CXX)
endif()

target_link_libraries(${library_name} m)

install(
    TARGETS ${library_name} LIBRARY
    DESTINATION ${destination}/lib
    COMPONENT Libraries)
install(
    FILES ${inc_files}
    DESTINATION ${destination}/include
    COMPONENT Headers)
