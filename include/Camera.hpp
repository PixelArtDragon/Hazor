#pragma once
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace tel {
class Camera {
  public:
    static Camera perspective(float fov, float aspectRatio, float nearClipping, float farClipping) {
        return Camera(glm::perspective(fov, aspectRatio, nearClipping, farClipping));
    }

    static Camera orthographic(float left, float top, float right, float bottom) {
        return Camera(glm::ortho(left, right, bottom, top));
    }

    [[nodiscard]] glm::mat4 matrix() const { return projection * transform; }

    glm::mat4 transform = glm::identity<glm::mat4>();

  private:
    explicit Camera(const glm::mat4& mat) : projection(mat) {}

    glm::mat4 projection;
};
} // namespace tel