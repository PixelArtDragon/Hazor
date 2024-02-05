#pragma once
#include "Initializations.hpp"
#include "RenderingHandles.hpp"

namespace tel {
struct Renderable {
    MustInit<ShaderHandle> shader;
    MustInit<MeshHandle> mesh;
};
} // namespace tel