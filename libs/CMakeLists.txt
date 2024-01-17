
set(BUILD_SHARED_LIBS OFF)
set(libs_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(magicube_libraries)

#======================================== SYSTEM LIBRARIES ========================================#

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

list(APPEND magicube_libraries OpenGL::GL Threads::Threads)

#=========================================== GLBINDING ============================================#

option(OPTION_BUILD_TOOLS "" OFF)
option(OPTION_BUILD_EXAMPLES "" OFF)
option(OPTION_USE_GIT_INFORMATION "" OFF)

add_subdirectory(${libs_dir}/glbinding)
list(APPEND magicube_libraries glbinding::glbinding)

#============================================== GLFW ==============================================#

option(GLFW_INSTALL OFF)

add_subdirectory(${libs_dir}/glfw)
list(APPEND magicube_libraries glfw)

#=============================================== GLM ==============================================#

add_subdirectory(${libs_dir}/glm)
list(APPEND magicube_libraries glm::glm)

#=============================================== FMT ==============================================#

add_subdirectory(${libs_dir}/fmt)
list(APPEND magicube_libraries fmt::fmt)

#============================================= SPDLOG =============================================#

option(SPDLOG_FMT_EXTERNAL "" ON)

add_subdirectory(${libs_dir}/spdlog)
list(APPEND magicube_libraries spdlog::spdlog)


#=========================================== MAGIC_ENUM ===========================================#

add_subdirectory(${libs_dir}/magic_enum)
list(APPEND magicube_libraries magic_enum::magic_enum)

#==================================================================================================#

add_library(magicube_dependencies INTERFACE)
add_library(magicube::dependencies ALIAS magicube_dependencies)
target_link_libraries(magicube_dependencies INTERFACE ${magicube_libraries})
