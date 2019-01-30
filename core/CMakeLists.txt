get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE files *.h *.cpp)
add_library(${lib} ${files})

target_include_directories(${lib}
    PRIVATE
    ${yaml_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
    ${CMAKE_SOURCE_DIR}
    )

message(STATUS "core to be linked with libcerf: ${Cerf_LIBRARIES}")
#message(STATUS "core to be linked with lmfit:   ${LMFit_LIBRARIES}")
message(STATUS "core to be linked with yaml:    ${yaml_LIBRARIES}")

target_link_libraries(${lib}
    PUBLIC
    qcr
    PRIVATE
    LevMar
    caress_loader
    mar_loader
    ${Cerf_LIBRARIES}
#    ${LMFit_LIBRARIES}
    ${yaml_LIBRARIES}
    )
