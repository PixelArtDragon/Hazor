#pragma once

#include "Moving.hpp"

namespace tel {
template <typename DataType>
class VertexBuffer {
  public:
    VertexBuffer() = default;

    using Type = DataType;

    static VertexBuffer create() {
        GLuint vbo{};
        glGenBuffers(1, &vbo);
        return VertexBuffer(vbo);
    }

    VertexBuffer(const VertexBuffer&) = delete;

    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept = default;

    VertexBuffer& operator=(VertexBuffer&& other) noexcept = default;

    ~VertexBuffer() { glDeleteBuffers(1, &vbo.value()); }

    [[nodiscard]] GLuint underlying() const { return vbo; }

  private:
    explicit VertexBuffer(GLuint vbo) : vbo(vbo) {}

    Moving<GLuint, 0, EngagedMoveAssignBehavior::Assert> vbo;
};
} // namespace tel