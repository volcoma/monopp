cmake_minimum_required(VERSION 3.0)

project (monopp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

option(BUILD_MONOPP_MONORT "Build the monort utility library" ON)
option(BUILD_MONOPP_TESTS "Build the tests" ON)

include(macros)

detect_platform()
print_info()

if(BUILD_MONOPP_TESTS)
	set_output_paths()
endif()

add_subdirectory(monopp)

if(BUILD_MONOPP_MONORT)	
	add_subdirectory(monort)
endif()

if(BUILD_MONOPP_TESTS)
	add_subdirectory(tests)
endif()
