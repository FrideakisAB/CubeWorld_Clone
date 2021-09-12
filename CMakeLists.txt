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
	
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHs")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHs")
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
	src/Engine.cpp
	src/GameScene.cpp
	src/GameObject.cpp

	src/Assets/Prefab.cpp
	src/Assets/AssetsManager.cpp

	src/Components/Camera.cpp
	src/Components/Transform.cpp
	src/Components/LightSource.cpp
	src/Components/ParticleSystem.cpp
	src/Components/MaterialComponent.cpp

	src/Systems/RenderSystem.cpp
	src/Systems/ParticleWorker.cpp

	src/Render/Mesh.cpp
	src/Render/Render.cpp
	src/Render/Shader.cpp
	src/Render/Texture.cpp
	src/Render/GLUtils.cpp
	src/Render/ParticleRender.cpp
	src/Render/ShadowsManager.cpp
	src/Render/ForwardPlusPipeline.cpp

	src/Utils/json.cpp
	src/Utils/Mathf.cpp
	src/Utils/Curve.cpp
	src/Utils/Gradient.cpp
	src/Utils/Primitives.cpp
	src/Utils/ShaderUtils.cpp

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

set(Editor_Source
	src/Editor/Editor.cpp
	src/Editor/CacheSystem.cpp

	src/Editor/Render/LinesMesh.cpp
	src/Editor/Render/EditorRender.cpp

	src/Editor/UI/MenuBar.cpp
	src/Editor/UI/LogViewer.cpp
	src/Editor/UI/IMenuEntry.cpp
	src/Editor/UI/GameWindow.cpp
	src/Editor/UI/SceneViewer.cpp
	src/Editor/UI/SceneEditor.cpp
	src/Editor/UI/AssetsViewer.cpp
	src/Editor/UI/EditorViewer.cpp

	src/Editor/UI/Viewers/MeshViewer.cpp
	src/Editor/UI/Viewers/LightViewer.cpp
	src/Editor/UI/Viewers/CameraViewer.cpp
	src/Editor/UI/Viewers/MaterialViewer.cpp
	src/Editor/UI/Viewers/ParticleViewer.cpp
	src/Editor/UI/Viewers/TransformViewer.cpp

	src/Editor/ImGui/ImGuizmo.cpp
	src/Editor/ImGui/imgui_dock.cpp
	src/Editor/ImGui/imgui_curve.cpp
	src/Editor/ImGui/imgui_custom.cpp
	src/Editor/ImGui/ImFileDialog.cpp
	src/Editor/ImGui/imgui_impl_glfw.cpp
	src/Editor/ImGui/imgui_impl_opengl3.cpp
	src/Editor/ImGui/imgui_color_gradient.cpp

	src/Editor/Resources/AssetsWriter.cpp
	src/Editor/Resources/ModelImporter.cpp
	src/Editor/Resources/TextureImporter.cpp

	src/Editor/Commands/CommandList.cpp
	src/Editor/Commands/EntityValidator.cpp
	src/Editor/Commands/ViewersCommands.cpp
	src/Editor/Commands/SceneViewCommands.cpp
	src/Editor/Commands/SceneEditorCommands.cpp
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
	CONAN_PKG::catch2
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

	add_executable(CubeWorld_Editor WIN32
		src/Editor/main.cpp
		${Source}
		${Editor_Source}
	)

	target_link_libraries(CubeWorld_Editor
		OpenGL::GL
		CONAN_PKG::stb
		CONAN_PKG::glm
		CONAN_PKG::glew
		CONAN_PKG::glfw
		CONAN_PKG::imgui
		CONAN_PKG::boost
		CONAN_PKG::assimp
		CONAN_PKG::nlohmann_json
	)
endif()