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
        include/Mesh.hpp
        include/Util.hpp
        include/Scene.hpp
        include/Transform.hpp
        include/RenderingHandles.hpp
        include/Initializations.hpp
        include/rendering_internals/VertexBuffer.hpp
        include/rendering_internals/Framebuffer.hpp
        include/rendering_internals/Shader.hpp
        include/rendering_internals/Shader.cpp
        include/rendering_internals/VertexArray.hpp
        include/rendering_internals/ElementBuffer.hpp
        include/rendering_internals/GPUMesh.hpp
        include/FileLoading.hpp
        src/FileLoading.cpp
        include/Renderable.hpp
        include/Camera.hpp
        include/MeshLoading.hpp
        src/MeshLoading.cpp)
target_link_libraries(hazor PUBLIC sol2)
target_link_libraries(hazor PUBLIC ${LUA_LIBRARIES})
target_link_libraries(hazor PUBLIC glfw)
target_link_libraries(hazor PUBLIC GLEW::glew)
target_link_libraries(hazor PUBLIC OpenGL::GL)
target_link_libraries(hazor PUBLIC glm::glm)
target_link_libraries(hazor PUBLIC assimp::assimp)
target_include_directories(hazor PUBLIC ${LUA_INCLUDE_DIR})
target_include_directories(hazor PUBLIC include)

cmrc_add_resource_library(hazor-resources NAMESPACE tel::data
        WHENCE data
        data/shaders/Main.vert
        data/shaders/Main.frag
        data/Test.obj
)

target_link_libraries(hazor PUBLIC hazor-resources)