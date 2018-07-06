file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (monort ${libsrc})

target_link_libraries(monort PUBLIC monopp)

target_include_directories(monort
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(monort PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
	WINDOWS_EXPORT_ALL_SYMBOLS ON
)

include(target_warning_support)
set_warning_level(monort ultra)

include(target_code_style_support)
set_code_style(monort snake_case)
