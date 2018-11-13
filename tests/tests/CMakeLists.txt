file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

set(target_name monopp_test)

add_executable (${target_name} ${libsrc})

target_link_libraries(${target_name} PUBLIC monort suitepp)

set_target_properties(${target_name} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO

)
if(MINGW)
	set_target_properties(${target_name} PROPERTIES
		LINK_FLAGS 
		"-static-libgcc -static-libstdc++ -static"
	)
endif()

target_compile_definitions(${target_name} PUBLIC DATA_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")


include(target_warning_support)
set_warning_level(${target_name} ultra)

include(target_code_style_support)
set_code_style(${target_name} lower_case check_headers "${extra_flags}")

enable_testing()
add_test(NAME ${target_name} COMMAND ${target_name})
