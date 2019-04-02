set(lib core)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

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
    devtools
    PRIVATE
    LevMar
    caress_loader
    mar_loader
    ${Cerf_LIBRARIES}
#    ${LMFit_LIBRARIES}
    ${yaml_LIBRARIES}
    )
