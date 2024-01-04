
set(magicube_libraries)

#======================================== SYSTEM LIBRARIES ========================================#

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

list(APPEND magicube_libraries OpenGL::GL Threads::Threads)

#==================================================================================================#

add_library(magicube_dependencies INTERFACE)
add_library(magicube::dependencies ALIAS magicube_dependencies)
target_link_libraries(magicube_dependencies INTERFACE ${magicube_libraries})
