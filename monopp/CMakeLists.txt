include(Mono)

set(_MONO_ASSEMBLY_DIR ${MONO_ASSEMBLY_PATH})
set(_MONO_CONFIG_DIR ${MONO_CONFIG_PATH})

set(INTERNAL_MONO_ASSEMBLY_DIR "${_MONO_ASSEMBLY_DIR}"
    CACHE PATH "Path to the Mono assembly dir (mono/lib). May be a relative path.")
set(INTERNAL_MONO_CONFIG_DIR "${_MONO_CONFIG_DIR}"
    CACHE PATH "Path to the Mono config dir (mono/etc). May be a relative path.")

configure_file(mono_build_config.h.in mono_build_config.h @ONLY)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library(monopp ${libsrc})

target_include_directories(monopp
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/..
        ${MONO_INCLUDE_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(monopp PRIVATE ${MONO_LIBRARIES})

set_target_properties(monopp PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Link this 'library' to use the following warnings
add_library(monopp_warnings INTERFACE)

if (MSVC)
	# Remove any set warning up to now
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	endif()
	target_compile_options(monopp_warnings INTERFACE /W4)
else()
	target_compile_options(monopp_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(monopp PRIVATE monopp_warnings)


