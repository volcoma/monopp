file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (managed_interface ${libsrc})

target_link_libraries(managed_interface PUBLIC monopp)

target_include_directories(managed_interface
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/..
)
