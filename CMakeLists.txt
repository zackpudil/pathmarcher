cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(marcher)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

if(MSVC) 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
	if(NOT WIN32)
		set(GLAD_LIBRARIES dl)
	endif()
endif()

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS} src/headers 
	vendor/glfw/include 
	vendor/glad/include)

file(GLOB VENDOR_SOURCES vendor/glad/src/glad.c)

file(GLOB PROJECT_SOURCES src/code/*.cpp)

file(GLOB PROJECT_HEADERS src/headers/*.hpp)

file(GLOB PROJECT_KERNELS src/kernels/*.cl)

file(GLOB PROJECT_SHADERS src/shaders/*.glsl)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Kernels" FILES ${PROJECT_KERNELS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDOR_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE 
	-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}/src\"
	-DCMAKE_BUILD_TYPE=Debug)

link_directories(${OpenCL_LIBRARY})
add_executable(${PROJECT_NAME} 
	${PROJECT_SOURCES} ${PROJECT_HEADERS}
	${PROJECT_KERNELS} ${PROJECT_SHADERS}
	${VENDOR_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARY} ${GLAD_LIBARIES} ${GLFW_LIBARIES} glfw)
