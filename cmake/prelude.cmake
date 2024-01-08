
#======================================= SETUP DIRECTORIES ========================================#

set(magicube_root ${CMAKE_CURRENT_SOURCE_DIR})

macro(make_directory_if_not_exists dir)
	if (NOT IS_DIRECTORY ${dir})
		file(MAKE_DIRECTORY ${dir})
	endif()
endmacro()

make_directory_if_not_exists(${magicube_root}/bin)
make_directory_if_not_exists(${magicube_root}/bin/assets)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${magicube_root}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${magicube_root}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${magicube_root}/bin)

#=========================================== UTILITIES ============================================#

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
	message(STATUS "Ccache found")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

#======================================= PLATFORM DETECTION =======================================#

cmake_host_system_information(RESULT magicube_platform QUERY OS_NAME)
string(TOLOWER ${magicube_platform} magicube_platform)

message(STATUS "Detected platform: ${magicube_platform}")

#========================================== CXX STANDARD ==========================================#

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

#========================================== DEFINITIONS ===========================================#

add_compile_definitions(MAGICUBE_$<IF:$<CONFIG:Debug>,DEBUG,RELEASE>)

#========================================= COMPILER FLAGS =========================================#
