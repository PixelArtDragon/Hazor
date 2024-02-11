#pragma once
#include "Initializations.hpp"
#include "Moving.hpp"

#include <GL/glew.h>
#include <algorithm>
#include <cassert>

namespace tel {
struct FramebufferOptions {
    MustInit<int> width;
    MustInit<int> height;
};

class Framebuffer {
  public:
    static Framebuffer create(const FramebufferOptions& options) {
        GLuint fbo{};
        glGenFramebuffers(1, &fbo);
        return Framebuffer{fbo, options};
    }

    static Framebuffer define_default_framebuffer(const FramebufferOptions& framebufferOptions) {
        return Framebuffer{framebufferOptions};
    }

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept = default;

    Framebuffer& operator=(Framebuffer&& other) = default;

    ~Framebuffer() { glDeleteFramebuffers(1, &fbo.value()); }

    [[nodiscard]] GLuint underlying() const { return fbo; }

    [[nodiscard]] int width() const { return options.width; }

    [[nodiscard]] int height() const { return options.height; }

  private:
    Moving<GLuint, 0, EngagedMoveAssignBehavior::Assert> fbo = 0;
    FramebufferOptions options;

    Framebuffer(GLuint fbo, const FramebufferOptions& options) : fbo(fbo), options(options) {}

    explicit Framebuffer(const FramebufferOptions& options) : options(options) {}
};
} // namespace tel