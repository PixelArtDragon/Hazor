#pragma once

#include "ElementBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "Util.hpp"
#include <tuple>

namespace tel {
struct GPUMesh {
    ElementBuffer elementBuffer;
    ElementBuffer::Index numIndices = 0;
    VertexArray vertexArray;
    std::tuple<VertexBuffer<glm::vec3>, VertexBuffer<glm::vec3>> attachments;
};
} // namespace tel