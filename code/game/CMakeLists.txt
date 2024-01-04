
file(GLOB_RECURSE magicube_game_sources CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(magicube_game STATIC ${magicube_game_sources})
add_library(magicube::game ALIAS magicube_game)

target_link_libraries(magicube_game PUBLIC
	magicube::dependencies
	magicube::core
)
set_target_properties(magicube_game PROPERTIES
	UNITY_BUILD ON
	UNITY_BUILD_BATCH_SIZE 100
)

