cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

if(NOT DEFINED TEST_NAME)
    message(FATAL_ERROR "TEST_NAME not specified")
endif()

if(NOT DEFINED STECA_SOURCE_DIR)
    message(FATAL_ERROR "STECA_SOURCE_DIR not specified")
endif()

if(NOT DEFINED STECA_BINARY_DIR)
    message(FATAL_ERROR "STECA_BINARY_DIR not specified")
endif()

set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/out/${TEST_NAME}")
set(ref_dir "${STECA_SOURCE_DIR}/ftest/ref/${TEST_NAME}")
set(Steca "${STECA_BINARY_DIR}/main/Steca")
set(test_exe "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.cmd")

message(DEBUG " --- run test ${name}")
message(DEBUG " --- source dir ${CMAKE_SOURCE_DIR}")
message(DEBUG " --- current source dir ${CMAKE_CURRENT_SOURCE_DIR}")
message(DEBUG " --- binary dir ${CMAKE_BINARY_DIR}")
message(DEBUG " --- current binary dir ${CMAKE_CURRENT_BINARY_DIR}")
message(DEBUG " --- Steca source dir ${STECA_SOURCE_DIR}")
message(DEBUG " --- Steca binary dir ${STECA_BINARY_DIR}")
message(DEBUG " --- out dir: ${out_dir}")
message(DEBUG " --- ref dir: ${ref_dir}")
message(DEBUG " --- Steca command: ${Steca}")
message(DEBUG " --- Test script: ${test_exe}")

# create directory where Steca writes its output files
file(REMOVE_RECURSE ${out_dir})
file(MAKE_DIRECTORY ${out_dir})

set(res FALSE) # without this, a timeout would appear as success

execute_process(
    COMMAND "${Steca}" "${test_exe}"
    TIMEOUT 300
    RESULT_VARIABLE res)

if(NOT res)
    message(DEBUG " --- Steca ran successfully")
else()
    message(FATAL_ERROR "Steca terminated with error: ${res}")
endif()

file(GLOB out_files "${out_dir}/*")
file(GLOB ref_files "${ref_dir}/*")

foreach(rfile ${ref_files})
    string(REGEX REPLACE ".*/" "" rname "${rfile}")
    string(APPEND rnames ${rname})
endforeach()

foreach(ofile ${out_files})
    string(REGEX REPLACE ".*/" "" oname "${ofile}")
    string(APPEND onames ${oname})
endforeach()

if(NOT ${rnames} STREQUAL ${onames})
    message(DEBUG " --- out files: ${onames}")
    message(DEBUG " --- ref files: ${rnames}")
    message(FATAL_ERROR "Output files don't match reference")
endif()

foreach(fnam ${onames})
    execute_process(COMMAND diff "${out_dir}/${fnam}" "${ref_dir}/${fnam}"
        RESULT_VARIABLE res)
    if(NOT res)
        message(DEBUG " --- out file ${fnam} agrees with reference")
    else()
        message(FATAL_ERROR "out file ${fnam} differs from reference")
    endif()
endforeach()
