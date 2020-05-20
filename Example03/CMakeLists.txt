cmake_minimum_required (VERSION 3.8)
project(Example03)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)

set(lib_files "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    link_directories(${PROJECT_SOURCE_DIR}/../Third-Party/lib/windows/msvc ${PROJECT_SOURCE_DIR}/../TinyEngine/build/${CMAKE_BUILD_TYPE})
    set(lib_files Msvcrt)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(UNIX)
		link_directories(${PROJECT_SOURCE_DIR}/../Third-Party/lib/linux/gcc ${PROJECT_SOURCE_DIR}/../TinyEngine/build)
		set(lib_files X11 pthread dl)
	else()
		link_directories(${PROJECT_SOURCE_DIR}/../Third-Party/lib/windows/gcc ${PROJECT_SOURCE_DIR}/../TinyEngine/build)
	endif()
endif()

add_executable(Example03 ${src_files})

target_include_directories(Example03 PRIVATE
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/glfw/include
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/glm
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/slimktx2/include
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/slimktx2/include
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/stb
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/tinygltf
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/volk
	$ENV{VULKAN_SDK}/include
	${PROJECT_SOURCE_DIR}/../TinyEngine/src
	${PROJECT_SOURCE_DIR}/../TinyEngine/src/gui/imgui
)

target_link_libraries(Example03 PRIVATE
	TinyEngine
	glfw3
	slimktx2
	volk
	shaderc_combined
	${lib_files}
)
