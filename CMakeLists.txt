cmake_minimum_required(VERSION 3.25)

project(hazor)
set(CMAKE_CXX_STANDARD 23)

find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)
find_package(assimp REQUIRED)
add_library(hazor
        src/Mesh.hpp
        src/ObjectPool.hpp
        src/Util.hpp
        src/Scene.hpp
        src/Transform.hpp
        src/RenderingHandles.hpp
        src/Initializations.hpp
        src/rendering_internals/VertexBuffer.hpp
        src/rendering_internals/Framebuffer.hpp
        src/rendering_internals/Shader.hpp
        src/rendering_internals/Shader.cpp
        src/rendering_internals/VertexArray.hpp
        src/rendering_internals/ElementBuffer.hpp
        src/rendering_internals/GPUMesh.hpp
        src/FileLoading.hpp
        src/FileLoading.cpp
        src/Renderable.hpp
        src/Camera.hpp
        src/MeshLoading.hpp
        src/MeshLoading.cpp)
target_link_libraries(hazor PRIVATE sol2)
target_link_libraries(hazor PRIVATE ${LUA_LIBRARIES})
target_link_libraries(hazor PRIVATE glfw)
target_link_libraries(hazor PRIVATE GLEW::glew)
target_link_libraries(hazor PRIVATE OpenGL::GL)
target_link_libraries(hazor PRIVATE glm::glm)
target_link_libraries(hazor PRIVATE assimp::assimp)
target_include_directories(hazor PRIVATE ${LUA_INCLUDE_DIR})

cmrc_add_resource_library(hazor-resources NAMESPACE tel::data
        WHENCE data
        data/shaders/Main.vert
        data/shaders/Main.frag
        data/Test.obj
)

target_link_libraries(hazor PRIVATE hazor-resources)