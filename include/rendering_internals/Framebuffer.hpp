#pragma once
#include "..\Initializations.hpp"

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

    Framebuffer(Framebuffer&& other) noexcept : fbo(other.fbo), options(std::move(other.options)) { other.fbo = 0; }

    Framebuffer& operator=(Framebuffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        assert(this->fbo == 0 && "Assignment to a Framebuffer that is engaged is not allowed");
        this->fbo = other.fbo;
        other.fbo = 0;
        this->options = std::move(other.options);
        return *this;
    }

    ~Framebuffer() { glDeleteFramebuffers(1, &fbo); }

    [[nodiscard]] GLuint underlying() const { return fbo; }

    [[nodiscard]] int width() const { return options.width; }

    [[nodiscard]] int height() const { return options.height; }

  private:
    GLuint fbo = 0;
    FramebufferOptions options;

    Framebuffer(GLuint fbo, const FramebufferOptions& options) : fbo(fbo), options(options) {}

    explicit Framebuffer(const FramebufferOptions& options) : options(options) {}
};
} // namespace tel