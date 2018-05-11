include(macros)

detect_platform()
print_info()
set_output_paths()

add_subdirectory(native)
add_subdirectory(managed)

