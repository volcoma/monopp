file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (managed_interface ${libsrc})

target_link_libraries(managed_interface PUBLIC monopp)

target_include_directories(managed_interface
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/..
)


set_target_properties(managed_interface PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
