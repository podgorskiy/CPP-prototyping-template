cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0048 OLD)

project(OpenGLExamples)

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_INSTALL OFF)
option (USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)

if (OPENGL_FOUND)
	message("OpenGL Correctly Found")
	include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
	message("OpenGL environment missing")
endif (OPENGL_FOUND)

##############################################################
# Generating OpenGL profile
##############################################################
message("Executing gl3w_gen.py")
execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/libs/gl3w WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

##############################################################
# Includes
##############################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/gl3w/include)
include_directories(${CMAKE_BINARY_DIR}/libs/gl3w/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)

##############################################################
# Externa libs
##############################################################
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)

#####################################################################
# imgui
#####################################################################
set(IMGUI_DIR libs/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h )
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp)

##############################################################
# Targets
##############################################################
file(GLOB_RECURSE SOURCES 17_2_imgui/sources/*.cpp src/*.h)
add_executable(imgui_template ${SOURCES})

##############################################################
# Linkage
##############################################################
target_link_libraries(imgui_template gl3w glfw imgui ${OPENGL_LIBRARIES})


#####################################################################
# Examples
#####################################################################
add_subdirectory (7_GLFW_Sierpinski)
add_subdirectory (9_1_GL3W_basic)
add_subdirectory (9_2_Triangle_VertexBuffers_CPU)
add_subdirectory (10_1_Triangle_VertexBuffers_GPU)
add_subdirectory (10_2_Rendering_Triangle_Colors)
add_subdirectory (10_3_Uniforms)
add_subdirectory (10_4_Uniforms_Rotation)
add_subdirectory (11_PolarCoordinates)
add_subdirectory (12_Mesh_Obj_File)
add_subdirectory (16_GLFW_input)
add_subdirectory (17_interaction_imgui)
add_subdirectory (18_lambert_shaders)
add_subdirectory (19_cubemap_normalmapping)
add_subdirectory (20_pbr)

