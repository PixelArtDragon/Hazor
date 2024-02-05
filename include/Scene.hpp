#pragma once
#include "Camera.hpp"
#include "Renderable.hpp"
#include "Transform.hpp"

namespace tel {
struct SceneObject {
    Transform transform;
    Renderable renderable;
};

struct Scene {
    std::vector<SceneObject> sceneObjects;
    Camera camera;
};
} // namespace tel