#pragma once

#include "Moving.hpp"

#include <GL/glew.h>

namespace tel {
class VertexArray {
  public:
    VertexArray() = default;

    static VertexArray create() {
        GLuint vao{};
        glGenVertexArrays(1, &vao);
        return VertexArray{vao};
    }

    VertexArray(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept = default;

    VertexArray& operator=(const VertexArray& other) = delete;

    VertexArray& operator=(VertexArray&& other) noexcept = default;

    [[nodiscard]] GLuint underlying() const { return vao; }

    ~VertexArray() { glDeleteVertexArrays(1, &vao.value()); }

  private:
    friend class Rendering;

    explicit VertexArray(GLuint vao) : vao(vao) {}

    Moving<GLuint, 0, EngagedMoveAssignBehavior::Assert> vao = 0;
};
} // namespace tel