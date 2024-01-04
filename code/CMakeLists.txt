
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)                          # magicube::core
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/game)                          # magicube::game
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platform/${magicube_platform}) # magicube::target

target_include_directories(magicube_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(magicube_target PUBLIC
	magicube::core
	magicube::game
)

set_target_properties(magicube_target PROPERTIES
	OUTPUT_NAME ${PROJECT_NAME}
)
