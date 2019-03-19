cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

if(NOT DEFINED name)
    message(FATAL_ERROR "test name not specified --- use -D name=...")
endif()

message(DEBUG " --- run test ${name}")

set(test_exe "${CMAKE_CURRENT_BINARY_DIR}/${name}.cmd")

# create directory where Steca writes its output files
set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/out/${name}")
file(REMOVE_RECURSE ${out_dir})
file(MAKE_DIRECTORY ${out_dir})

set(ref_dir "${CMAKE_CURRENT_SOURCE_DIR}/ref/${name}")

execute_process(COMMAND /G/Steca/build/main/Steca /G/Steca/ftest/1.cmd
    RESULT_VARIABLE res)

if(NOT res)
    message(DEBUG " --- Steca terminated successfully")
else()
    message(DEBUG " --- Steca command: ${CMAKE_BINARY_DIR}/../main/Steca")
    message(DEBUG " --- Steca argument: ${test_exe}")
    message(DEBUG " --- Steca output: ${sout}")
    message(DEBUG " --- Steca error: ${serr}")
    message(FATAL_ERROR "Steca terminated with error")
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
