file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (tests ${libsrc})

target_link_libraries(tests PUBLIC monort)

set_target_properties(tests PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
if(MINGW)
	set_target_properties(tests PROPERTIES 
		LINK_FLAGS 
		"-static-libgcc -static-libstdc++ -static"
	)
endif()

include(target_warning_support)
set_warning_level(tests high)
