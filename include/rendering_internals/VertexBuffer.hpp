#pragma once

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

    VertexBuffer(VertexBuffer&& other) noexcept : vbo(other.vbo) { other.vbo = 0; }

    VertexBuffer& operator=(VertexBuffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        assert(this->vbo == 0 && "Assignment to a VertexBufferArray that is engaged is not allowed");
        this->vbo = other.vbo;
        other.vbo = 0;
        return *this;
    }

    ~VertexBuffer() { glDeleteBuffers(1, &vbo); }

    [[nodiscard]] GLuint underlying() const { return vbo; }

  private:
    explicit VertexBuffer(GLuint vbo) : vbo(vbo) {}

    GLuint vbo = 0;
};
} // namespace tel