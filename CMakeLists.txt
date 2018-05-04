cmake_minimum_required(VERSION 3.0)

project (monopp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

include(macros)
include(sanitizers_support)
include(clang_tidy_support)
include(ide_support)

#enable_sanitizers("custom")

detect_platform()
print_info()
set_output_paths()

set_project_custom_defines()

add_subdirectory(monopp)
add_subdirectory(monort)
add_subdirectory(tests)
