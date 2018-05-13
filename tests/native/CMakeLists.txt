file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (native ${libsrc})

target_link_libraries(native PUBLIC monort)

set_target_properties(native PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
if(MINGW)
	set_target_properties(native PROPERTIES 
		LINK_FLAGS 
		"-static-libgcc -static-libstdc++ -static"
	)
endif()

include(target_warning_support)
set_warning_level(native high)
