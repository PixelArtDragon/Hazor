#pragma once
#include <GL/glew.h>

namespace tel {
class ElementBuffer {
  public:
    using Index = unsigned int;

    ElementBuffer() = default;

    ElementBuffer(const ElementBuffer& other) = delete;

    ElementBuffer(ElementBuffer&& other) noexcept = default;

    ElementBuffer& operator=(const ElementBuffer& other) = delete;

    ElementBuffer& operator=(ElementBuffer&& other) noexcept = default;

    ~ElementBuffer() { glDeleteBuffers(1, &ebo); }

    static ElementBuffer create() {
        GLuint ebo{};
        glGenBuffers(1, &ebo);
        return ElementBuffer(ebo);
    }

    [[nodiscard]] GLuint underlying() const { return ebo; }

  private:
    friend class Rendering;

    explicit ElementBuffer(GLuint ebo) : ebo(ebo) {}

    Moving<GLuint, 0, EngagedMoveAssignBehavior::Assert> ebo;
};
} // namespace tel