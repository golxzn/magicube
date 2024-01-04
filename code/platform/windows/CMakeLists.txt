
add_executable(magicube_target ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_executable(magicube::target ALIAS magicube_target)

set_target_properties(magicube_target PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${magicube_root}/bin
)

install(FILES $<TARGET_RUNTIME_DLLS:magicube_target> TYPE BIN)
