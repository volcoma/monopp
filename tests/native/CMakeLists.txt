file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (native ${libsrc})

target_link_libraries(native PUBLIC monopp)

