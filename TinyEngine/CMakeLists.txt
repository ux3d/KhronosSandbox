cmake_minimum_required (VERSION 3.8)
project(TinyEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
endif()
add_definitions(-DIMGUI_DISABLE_FILE_FUNCTIONS)

file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)

add_library(TinyEngine STATIC ${src_files})

target_include_directories(TinyEngine PRIVATE
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/glfw/include
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/glm
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/slimktx2/include
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/slimktx2/include
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/stb
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/tinygltf
	${PROJECT_SOURCE_DIR}/../Third-Party/GitHub/volk
	$ENV{VULKAN_SDK}/include
	${PROJECT_SOURCE_DIR}/src/gui/imgui
)
