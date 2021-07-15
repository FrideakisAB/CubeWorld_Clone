cmake_minimum_required(VERSION 3.16)

project(CubeWorld_Clone CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(include)

find_package(OpenGL REQUIRED)

# Исправление Conan при компиляции Clang под MSVC
set(PROJECT_IS_MSVC_CLANG (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT} STREQUAL "MSVC"))

if(PROJECT_IS_MSVC_CLANG AND NOT CMAKE_CXX_COMPILER_ID MATCHES GNU)
	set(PREV_CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
	set(PREV_CMAKE_CXX_GENERATOR_TOOLSET ${CMAKE_CXX_GENERATOR_TOOLSET})
	set(CMAKE_CXX_COMPILER_ID MSVC)
	set(CMAKE_CXX_GENERATOR_TOOLSET ClangCL)
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt 
				INSTALL_FOLDER ${CMAKE_BINARY_DIR}/pkgs
				BASIC_SETUP
				CMAKE_TARGETS
				BUILD missing
)

if(PROJECT_IS_MSVC_CLANG AND NOT CMAKE_CXX_COMPILER_ID MATCHES GNU)
	set(CMAKE_CXX_COMPILER_ID ${PREV_CMAKE_CXX_COMPILER_ID})
	set(CMAKE_CXX_GENERATOR_TOOLSET ${PREV_CMAKE_CXX_GENERATOR_TOOLSET})
endif()

set(Source
	src/Log.cpp

	src/Assets/AssetsManager.cpp

	src/Components/Camera.cpp
	src/Components/Material.cpp
	src/Components/Transform.cpp
	src/Components/LightSource.cpp

	src/Systems/RenderSystem.cpp

	src/Render/Mesh.cpp
	src/Render/Render.cpp
	src/Render/GLUtils.cpp

	src/Utils/json.cpp

	src/ECS/API.cpp
	src/ECS/Engine.cpp
	src/ECS/IEntity.cpp
	src/ECS/ISystem.cpp
	src/ECS/IComponent.cpp
	src/ECS/EntityManager.cpp
	src/ECS/SystemManager.cpp
	src/ECS/ComponentManager.cpp

	src/ECS/Event/IEvent.cpp
	src/ECS/Event/EventHandler.cpp
	src/ECS/Event/IEventListener.cpp

	src/ECS/util/Timer.cpp
	src/ECS/util/FamilyTypeID.cpp

	src/Memory/MemoryManager.cpp
	src/Memory/GlobalMemoryUser.cpp

	src/Memory/Allocator/IAllocator.cpp
	src/Memory/Allocator/PoolAllocator.cpp
	src/Memory/Allocator/StackAllocator.cpp
	src/Memory/Allocator/LinearAllocator.cpp
	src/Memory/Allocator/ListLinearAllocator.cpp
)

add_executable(tests
	tests/test.cpp
	tests/ECSTests.h

	${Source}
)

target_link_libraries(tests
	OpenGL::GL
	CONAN_PKG::stb
	CONAN_PKG::glm
	CONAN_PKG::boost
	CONAN_PKG::glew
	CONAN_PKG::glfw
	CONAN_PKG::assimp
	CONAN_PKG::Catch2
	CONAN_PKG::nlohmann_json
)

if(WIN32)
	add_executable(CubeWorld_App WIN32
		src/main.cpp
		${Source}
	)

	target_link_libraries(CubeWorld_App
		OpenGL::GL
		CONAN_PKG::stb
		CONAN_PKG::glm
		CONAN_PKG::boost
		CONAN_PKG::glew
		CONAN_PKG::glfw
		CONAN_PKG::assimp
		CONAN_PKG::nlohmann_json
	)
endif()