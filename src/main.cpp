#include "Engine.hpp"
#include "MeshLoading.hpp"
#include "Renderable.hpp"

#include <assimp/mesh.h>
#include <cmrc/cmrc.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sol/sol.hpp>

CMRC_DECLARE(tel::data);

int main(int, char**) {
    const auto fs = cmrc::tel::data::get_filesystem();
    auto vertexFile = fs.open("shaders/Main.vert");
    auto fragmentFile = fs.open("shaders/Main.frag");

    auto meshFile = fs.open("Test.obj");
    auto testMesh = tel::load_mesh_from_memory(std::string_view(meshFile));

    tel::Engine engine{};
    const auto meshHandle = engine.rendering_system().load_mesh(testMesh.value());
    const auto shaderHandle =
        engine.rendering_system().load_shader(std::string_view(vertexFile), std::string_view(fragmentFile));
    if (!shaderHandle) {
        std::cout << shaderHandle.error().text.data() << std::endl;
        return 1;
    }
    engine.current_scene().sceneObjects.emplace_back(
        glm::rotate(glm::identity<glm::mat4>(), glm::pi<float>() / 3.0f, normalize(glm::vec3(1.0f, 1.0f, 0.0f))),
        tel::Renderable{.shader = shaderHandle.value(), .mesh = meshHandle});
    engine.current_scene().camera.transform =
        glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    engine.start_main_loop();
    return 0;
}
