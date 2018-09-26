set(lib cerf)
if (${CERF_CPP})
    set(lib cerfcpp)
endif()
message(STATUS "libcerf: build library ${lib}")

set(cerf_LIBRARY ${lib} PARENT_SCOPE)

set(src_files erfcx.c err_fcts.c im_w_of_x.c w_of_z.c)
set(inc_files cerf.h defs.h)

if (${CERF_CPP})
    set_property(SOURCE ${src_files} PROPERTY LANGUAGE CXX)
endif()

add_library(${lib} SHARED ${src_files})

set_target_properties(
    ${lib} PROPERTIES
    OUTPUT_NAME ${lib}
    VERSION ${CERF_VERSION}
    SOVERSION ${CERF_SOVERSION})
if (${CERF_CPP})
    set_property(TARGET ${lib} PROPERTY LINKER_LANGUAGE CXX)
endif()

target_link_libraries(${lib} m)

if (${LIB_INSTALL})
    install(
        TARGETS ${lib} LIBRARY
        DESTINATION ${destination}/lib
        COMPONENT Libraries)
    install(
        FILES ${inc_files}
        DESTINATION ${destination}/include
        COMPONENT Headers)
endif()
