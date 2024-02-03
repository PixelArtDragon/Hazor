#pragma once

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

    VertexArray(VertexArray&& other) noexcept : vao{other.vao} { other.vao = 0; }

    VertexArray& operator=(const VertexArray& other) = delete;

    VertexArray& operator=(VertexArray&& other) noexcept {
        if (this == &other)
            return *this;
        vao = other.vao;
        other.vao = 0;
        return *this;
    }

    [[nodiscard]] GLuint underlying() const { return vao; }

    ~VertexArray() { glDeleteVertexArrays(1, &vao); }

  private:
    friend class Rendering;

    explicit VertexArray(GLuint vao) : vao(vao) {}

    GLuint vao = 0;
};
} // namespace tel