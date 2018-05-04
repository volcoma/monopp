file(GLOB_RECURSE libsharp *.cs)

add_mono_assembly(
    TARGET managed_lib
    DESTINATION ${CMAKE_BINARY_DIR}/bin
    TYPE library
    SOURCES ${libsharp}
)
