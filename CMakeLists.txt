cmake_minimum_required(VERSION 3.0)

project (monopp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

option(BUILD_SHARED_LIBS "Build itc as a shared library." ON)
option(BUILD_MONOPP_MONORT "Build the monort utility library" ON)
option(BUILD_MONOPP_TESTS "Build the tests" ON)
option(BUILD_WITH_CODE_STYLE_CHECKS "Build with code style checks." OFF)

if(BUILD_MONOPP_TESTS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin) 
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(BUILD_MONOPP_MONORT ON)
	message(STATUS "Enabled monopp tests.")
endif()

add_subdirectory(monopp)

if(BUILD_MONOPP_MONORT)	
	add_subdirectory(monort)
endif()

if(BUILD_MONOPP_TESTS)
	add_subdirectory(tests)
endif()
