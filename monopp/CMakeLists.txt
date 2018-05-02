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
		${CMAKE_CURRENT_SOURCE_DIR}
        ${MONO_INCLUDE_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(monopp ${MONO_LIBRARIES})
