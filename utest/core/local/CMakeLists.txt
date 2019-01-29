# Directory "local" contains test must not be linked with the library under test.

# We glob test sources, though this is often considered evil.
# Just touch CMakeLists.txt each time a test is added or withdrawn.

file(GLOB test_sources "[0-9]*.cpp")
list(SORT test_sources)

foreach(test_src ${test_sources})
    # remove directory path and suffix, to retain just the name of the test
    string(REGEX REPLACE ".*/" "" test_name "${test_src}")
    string(REGEX REPLACE "\\.cpp$" "" test_name "${test_name}")
    set(test_name "core${test_name}")
    my_cxx_test(${test_name} ${test_src} "")
    # TODO: Qt only needed as include directory, not as linked library
    target_include_directories(${test_name} PRIVATE ${inc_dirs})
    target_link_libraries(${test_name} PRIVATE gtest_main Qt5::Core)
endforeach(test_src)
