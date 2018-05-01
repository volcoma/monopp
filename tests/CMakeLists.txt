file(GLOB_RECURSE libsharp *.cs)

add_mono_assembly(
    TARGET managed_lib
    DESTINATION ${CMAKE_BINARY_DIR}/bin
    TYPE library
    SOURCES ${libsharp}
)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (monopp_tests ${libsrc})

target_link_libraries(monopp_tests PUBLIC monopp)

add_dependencies(monopp_tests managed_lib)
