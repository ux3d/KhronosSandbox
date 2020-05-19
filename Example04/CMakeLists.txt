cmake_minimum_required (VERSION 3.8)
project(Example04)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}) 
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}) 
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}) 
endif()

file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)

add_executable(Example04 ${src_files})

target_include_directories(Example04 PRIVATE
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

set(lib_files "")

if(MSVC)
    target_link_directories(Example04 PRIVATE ${PROJECT_SOURCE_DIR}/../Third-Party/lib/windows/msvc ${PROJECT_SOURCE_DIR}/../TinyEngine/build/${CMAKE_BUILD_TYPE})
	set(lib_files Msvcrt)
endif()

target_link_libraries(Example04 PRIVATE
	glfw3
	slimktx2
	volk
	shaderc_combined
	TinyEngine
	${lib_files}
)
