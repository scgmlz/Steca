set(lib devtools)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${SMAKE_SOURCE_DIR}/3rdparty
    )
