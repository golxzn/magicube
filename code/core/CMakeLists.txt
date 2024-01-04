
file(GLOB_RECURSE magicube_core_sources CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(magicube_core STATIC ${magicube_core_sources})
add_library(magicube::core ALIAS magicube_core)

target_link_libraries(magicube_core PUBLIC magicube::dependencies)
set_target_properties(magicube_core PROPERTIES
	UNITY_BUILD ON
	UNITY_BUILD_BATCH_SIZE 100
)
