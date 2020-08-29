cmake_minimum_required(VERSION 3.6)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0048 NEW)

project(imgui_template)

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
# Setup compiler options and configurations
##############################################################

if(MSVC)
    set(COMMON_OPTIONS /W3 /wd4996 /we4239 /we4002 /we4700 /we4305 /EHsc /MP)
    set(DEBUG_OPTIONS /MTd /Od /Zi)
    set(RELEASE_OPTIONS /MT /Ox /fp:fast /GL /GR-)

    add_definitions(-D_SCL_SECURE)
    add_definitions(-D_SECURE_SCL)
    add_definitions(-D_ALLOW_KEYWORD_MACROS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-DWINVER=0x0602)
    add_definitions(-D_WIN32_WINNT=0x0602)
    add_definitions(-DNTDDI_VERSION=0x06020000)

elseif(APPLE)
    set(COMMON_OPTIONS -DHAVE_PTHREAD -DGUID_CFUUID -lstdc++fs)
    set(DEBUG_OPTIONS -g)
    set(RELEASE_OPTIONS -Ofast )
else()
    set(COMMON_OPTIONS -DHAVE_PTHREAD  -lstdc++fs)
    set(RELEASE_OPTIONS -O2)
endif()

set(DEBUG_OPTIONS ${DEBUG_OPTIONS} ${COMMON_OPTIONS})
set(RELEASE_OPTIONS ${RELEASE_OPTIONS} ${COMMON_OPTIONS})

add_compile_options(
        "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>"
        "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO")
endif()

add_definitions(-D__STDC_LIMIT_MACROS)
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-D__STDC_CONSTANT_MACROS)

add_definitions(-DHAVE_SSE42)

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS "-fPIC -g  ${CMAKE_C_FLAGS}")
set (CMAKE_CXX_FLAGS "-fPIC -g -fno-strict-aliasing -msse2 -msse3 -msse4 -funsafe-math-optimizations ${CMAKE_CXX_FLAGS}")

#####################################################################
# Includes
#####################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/gl3w/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/doctest/doctest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/fsal/sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/lz4/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/yaml-cpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources)

include_directories(${CMAKE_BINARY_DIR}/libs/gl3w/include)
include_directories(${CMAKE_BINARY_DIR}/libs/gl3w/include)
include_directories(${PROJECT_BINARY_DIR})
#####################################################################


##############################################################
# Externals
##############################################################
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)
add_subdirectory (libs/fsal)
add_subdirectory (libs/yaml-cpp)

target_include_directories(fsal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/lz4/lib)

#####################################################################
# imgui
#####################################################################
set(IMGUI_DIR libs/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h)
add_library(imgui  ${IMGUI_SOURCES}
                   ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
                   ${IMGUI_DIR}/examples/imgui_impl_glfw.h
                   ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
                   ${IMGUI_DIR}/examples/imgui_impl_opengl3.h)
#####################################################################


#####################################################################
# lz4
#####################################################################
set(LZ4_DIR libs/lz4/lib/)
set(SOURCES_LZ4 ${LZ4_DIR}lz4.c ${LZ4_DIR}lz4hc.c ${LZ4_DIR}lz4.h ${LZ4_DIR}lz4hc.h ${LZ4_DIR}xxhash.c ${LZ4_DIR}xxhash.h)
add_library(lz4 ${SOURCES_LZ4})
#####################################################################


#####################################################################
# zlib
#####################################################################
file(GLOB ZLIB_SOURCES libs/zlib/*.c libs/zlib/*.h)
add_library(zlib_static STATIC ${ZLIB_SOURCES})
#####################################################################


#####################################################################
# pugixml
#####################################################################
file(GLOB PUGIXML_SOURCES libs/pugixml/src/*.cpp libs/pugixml/src/*.hpp)
add_library(pugixml ${PUGIXML_SOURCES})
#####################################################################


#####################################################################
# Main sources
#####################################################################
file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h sources/*.c sources/*.cc)
############# ########################################################


#####################################################################
# Targets
#####################################################################
add_executable(imgui_template ${SOURCES})
#####################################################################

#####################################################################
# Linkage
#####################################################################
set(LIBRARIES rt m  stdc++fs gomp  ${PYTHON_LIBRARY} glfw gl3w imgui pthread dl pugixml fsal lz4 zlib_static yaml-cpp ${OPENGL_LIBRARIES} )
target_link_libraries(imgui_template PRIVATE ${LIBRARIES})
SET_TARGET_PROPERTIES(imgui_template PROPERTIES PREFIX "_")
#####################################################################

if(MSVC)
else()
	target_link_libraries(imgui_template PRIVATE stdc++fs)
	target_compile_options(imgui_template PRIVATE -lstdc++fs -Wno-switch)
	target_link_options(imgui_template PRIVATE -static-libstdc++)
endif()
