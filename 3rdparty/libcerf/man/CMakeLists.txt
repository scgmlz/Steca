function(one_page pname section srcname)
    add_custom_command(
        OUTPUT "${pname}.${section}"
        COMMAND pod2man -s ${section} -c "libcerf manual"
            "${CMAKE_CURRENT_SOURCE_DIR}/${srcname}.pod"
            "${CMAKE_CURRENT_BINARY_DIR}/${pname}.${section}"
        DEPENDS ${srcname}.pod
        )
    add_custom_command(
        OUTPUT ${pname}.html
        COMMAND pod2html --title="libcerf manual" --noindex
            ${CMAKE_CURRENT_SOURCE_DIR}/${srcname}.pod
            > ${CMAKE_CURRENT_BINARY_DIR}/${pname}.html
        DEPENDS ${srcname}.pod
        )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${pname}.${section}
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man${section}"
        )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${pname}.html
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/libcerf/html"
        )
    set(TARGETS_MAN "${TARGETS_MAN};${pname}.${section}" PARENT_SCOPE)
    set(TARGETS_HTM "${TARGETS_HTM};${pname}.html"       PARENT_SCOPE)
endfunction()

set(TARGETS_MAN "")
set(TARGETS_HTM "")

one_page(cerf       3 cerf)
one_page(cerfc      3 cerf)
one_page(dawson     3 dawson)
one_page(cdawson    3 dawson)
one_page(erfcx      3 erfcx)
one_page(cerfcx     3 erfcx)
one_page(erfi       3 erfi)
one_page(cerfi      3 erfi)
one_page(voigt      3 voigt)
one_page(voigt_hwhm 3 voigt_hwhm)
one_page(w_of_z     3 w_of_z)
one_page(im_w_of_z  3 w_of_z)

# message(STATUS "targets man: ${TARGETS_MAN}")
# message(STATUS "targets htm: ${TARGETS_HTM}")
add_custom_target(man  ALL DEPENDS ${TARGETS_MAN})
add_custom_target(html ALL DEPENDS ${TARGETS_HTM})
