cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(pathmarcher)
add_subdirectory(vendor/opencv)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS} src/headers)

file(GLOB PROJECT_SOURCES src/code/*.cpp)
file(GLOB PROJECT_HEADERS src/headers/*.hpp)
file(GLOB PROJECT_KERNELS src/kernels/*.cl src/kernels/**/*.cl)
file(GLOB PROJECT_SHADERS src/shaders/*.glsl)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Kernels" FILES ${PROJECT_KERNELS})
source_group("Sources" FILES ${PROJECT_SOURCES})

add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}/src\" "-w")

link_directories(${OpenCL_LIBRARY})

add_executable(${PROJECT_NAME} 
	${PROJECT_SOURCES} ${PROJECT_HEADERS}
	${PROJECT_KERNELS} ${PROJECT_SHADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARY})
