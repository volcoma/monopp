file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (native ${libsrc})

target_link_libraries(native PUBLIC managed_interface)

set_target_properties(native PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
