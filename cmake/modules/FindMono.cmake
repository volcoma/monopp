if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
	set (MONO_DEFAULT_INSTALL_PATH
		"C:/Program Files"
	)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
	set (MONO_DEFAULT_INSTALL_PATH
		"C:/Program Files (x86)"
	)
endif()


if (WIN32)
	message(STATUS "Search path is: ${MONO_DEFAULT_INSTALL_PATH}")
endif()


find_program(
    MONO_EXECUTABLE
		mono
    PATHS
		${MONO_DEFAULT_INSTALL_PATH}/Mono/bin
		/bin
		/usr/bin
		/usr/local/bin
	DOC
		"The mono executable"

)

if (WIN32)
    find_program(
        MONO_MCS_EXECUTABLE
			mcs.bat
        PATHS
			${MONO_DEFAULT_INSTALL_PATH}/Mono/bin
			/bin
			/usr/bin
			/usr/local/bin
    )
else ()
    find_program(
		MONO_MCS_EXECUTABLE
			mcs
        PATHS
			${MONO_DEFAULT_INSTALL_PATH}/Mono/bin
			/bin
			/usr/bin
			/usr/local/bin
    )
endif ()

find_program(
    MONO_PKG_CONFIG_EXECUTABLE
		pkg-config
	PATHS
		${MONO_DEFAULT_INSTALL_PATH}/Mono/bin
		/bin
		/usr/bin
		/usr/local/bin
)

find_library(
    MONO_MAIN_LIBRARY NAMES mono-2.0 monosgen-2.0 mono-2.0-sgen
    PATHS
		${MONO_DEFAULT_INSTALL_PATH}/Mono/lib
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/opt/local/lib
	DOC
		"The mono-2.0 library"
)

# Determine the library prefix and suffix based on the operating system
if(WIN32)
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".lib")
elseif(APPLE)
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".dylib")
else()
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".so")
endif()

# Possible library names in prioritized order
set(LIB_NAMES
    "mono-2.0"
    "monosgen-2.0"
    "mono-2.0-sgen"
)

# Variable to store the found library
set(MONO_PATCH_MAIN_LIBRARY "")

# Function to search recursively for a library
function(find_library_recursively result lib_names base_path prefix suffix)
    # Find all potential files recursively
    file(GLOB_RECURSE all_files "${base_path}/*")
    foreach(file_path IN LISTS all_files)
        # Check if this file matches any library name with the given prefix and suffix
        foreach(lib_name IN LISTS ${lib_names})
            string(FIND "${file_path}" "${prefix}${lib_name}${suffix}" match_pos)
            if(match_pos GREATER -1)
                set(${result} "${file_path}" PARENT_SCOPE)
                return() # Exit as soon as the first match is found
            endif()
        endforeach()
    endforeach()
endfunction()

# Call the function to find the library
find_library_recursively(
    MONO_PATCH_MAIN_LIBRARY
    LIB_NAMES
    ${MONO_BINARY_PATCH_PATH}
    "${LIB_PREFIX}"
    "${LIB_SUFFIX}"
)

# Output the found library for debugging
message(STATUS "Found library: ${MONO_PATCH_MAIN_LIBRARY}")

#find_path( mono-2.0_INCLUDE_PATH mono/jit/jit.h
#		${MONO_DEFAULT_INSTALL_PATH}/Mono/include/*
#		/usr/include/mono-2.0
#		/usr/local/include
#		/opt/local/include
#		DOC "The directory where mono/jit/jit.h resides")

set(MONO_FOUND FALSE CACHE INTERNAL "")

if(MONO_EXECUTABLE AND MONO_MCS_EXECUTABLE AND MONO_PKG_CONFIG_EXECUTABLE AND MONO_MAIN_LIBRARY)
	set(MONO_FOUND TRUE CACHE INTERNAL "")

	get_filename_component(MONO_LIBRARY_PATH "${MONO_MAIN_LIBRARY}" DIRECTORY)
	set(MONO_LIBRARY_PATH "${MONO_LIBRARY_PATH}" CACHE PATH "")

	get_filename_component(MONO_INCLUDE_PATH "${MONO_LIBRARY_PATH}/../include/mono-2.0" ABSOLUTE)
	set(MONO_INCLUDE_PATH "${MONO_INCLUDE_PATH}" CACHE PATH "")
	set(MONO_ASSEMBLY_PATH "${MONO_LIBRARY_PATH}" CACHE PATH "")

	get_filename_component(MONO_CONFIG_PATH "${MONO_LIBRARY_PATH}/../etc" ABSOLUTE)
	set(MONO_CONFIG_PATH "${MONO_CONFIG_PATH}" CACHE PATH "")

	get_filename_component(MONO_BINARY_PATH "${MONO_LIBRARY_PATH}/../bin" ABSOLUTE)
	set(MONO_BINARY_PATH "${MONO_BINARY_PATH}" CACHE PATH "")


    execute_process(COMMAND "${MONO_MCS_EXECUTABLE}" "--version" OUTPUT_VARIABLE MONO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE ".*version ([^ ]+)" "\\1" MONO_VERSION "${MONO_VERSION}")
    message(STATUS "Found Mono version: ${MONO_MAIN_LIBRARY} (found version \"${MONO_VERSION}\")")


	if(MONO_PATCH_MAIN_LIBRARY)
		message(STATUS "Found Mono patch version: ${MONO_PATCH_MAIN_LIBRARY} (found version \"${MONO_VERSION}\")")
		set(MONO_MAIN_LIBRARY ${MONO_PATCH_MAIN_LIBRARY})
	endif()
	set(MONO_LIBRARIES "${MONO_MAIN_LIBRARY}")

    if (APPLE)
        find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
        set(MONO_LIBRARIES "${MONO_LIBRARIES};${CORE_FOUNDATION_LIBRARY}")
    endif ()

	message(STATUS "MONO_INCLUDE_PATH ${MONO_INCLUDE_PATH}")
	message(STATUS "MONO_LIBRARY_PATH ${MONO_LIBRARY_PATH}")
	message(STATUS "MONO_BINARY_PATH ${MONO_BINARY_PATH}")
	message(STATUS "MONO_LIBRARIES ${MONO_LIBRARIES}")
    message(STATUS "MONO_EXECUTABLE ${MONO_EXECUTABLE}")
    message(STATUS "MONO_MCS_EXECUTABLE ${MONO_MCS_EXECUTABLE}")
    message(STATUS "MONO_PKG_CONFIG_EXECUTABLE ${MONO_PKG_CONFIG_EXECUTABLE}")
    message(STATUS "MONO_MAIN_LIBRARY ${MONO_MAIN_LIBRARY}")

endif()

