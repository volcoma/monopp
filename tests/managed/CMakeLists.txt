file(GLOB_RECURSE libsharp *.cs)

add_mono_assembly(
    TARGET tests_managed
    DESTINATION ${CMAKE_BINARY_DIR}/bin
    TYPE library
    SOURCES ${libsharp}
	ADDITIONAL_ARGS "/debug:full;/reference:monort_managed.dll"
)

add_dependencies(tests_managed monort_managed)
