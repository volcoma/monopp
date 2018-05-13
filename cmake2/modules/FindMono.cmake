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
	
	set(MONO_LIBRARIES "${MONO_MAIN_LIBRARY}" CACHE STRING "")

    if (APPLE)
        find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
        set(MONO_LIBRARIES "${MONO_LIBRARIES};${CORE_FOUNDATION_LIBRARY}")
    endif ()
	
    execute_process(COMMAND "${MONO_MCS_EXECUTABLE}" "--version" OUTPUT_VARIABLE MONO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE ".*version ([^ ]+)" "\\1" MONO_VERSION "${MONO_VERSION}")
    message(STATUS "Found Mono version: ${MONO_MAIN_LIBRARY} (found version \"${MONO_VERSION}\")")
	
	message(STATUS ${MONO_INCLUDE_PATH})
	message(STATUS ${MONO_LIBRARY_PATH})
	message(STATUS ${MONO_BINARY_PATH})
	message(STATUS ${MONO_LIBRARIES})
endif()

