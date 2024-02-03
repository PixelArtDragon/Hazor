#pragma once

#include "rendering_internals/Framebuffer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace tel {
class Window {
  public:
    Window(int width, int height, const char* title)
        : width(width), height(height), defaultFramebuffer(Framebuffer::define_default_framebuffer(
                                            FramebufferOptions{.width = width, .height = height})) {
        initialize_glfw();
        set_window_hints();
        window = std::unique_ptr<GLFWwindow, Deleter>(glfwCreateWindow(width, height, title, nullptr, nullptr));
        glfwMakeContextCurrent(window.get());
        initialize_glew();
        assert(window);
        glfwGetFramebufferSize(window.get(), &renderWidth, &renderHeight);
        glfwSetWindowUserPointer(window.get(), this);
        register_callbacks();
    }

    [[nodiscard]] bool should_close() const { return glfwWindowShouldClose(window.get()); }

    void poll_events() { glfwPollEvents(); }

    void swap_buffers() { glfwSwapBuffers(window.get()); }

    [[nodiscard]] int get_render_width() const { return renderWidth; }

    [[nodiscard]] int get_render_height() const { return renderHeight; }

    [[nodiscard]] std::span<const int> pressed_keys() const { return keysPressed; }

    [[nodiscard]] std::span<const int> released_keys() const { return keysReleased; }

    [[nodiscard]] const Framebuffer& default_framebuffer() const { return defaultFramebuffer; }

  private:
    struct Deleter {
        void operator()(GLFWwindow* window) const { glfwDestroyWindow(window); }
    };

    int renderWidth{};
    int renderHeight{};
    int width;
    int height;
    std::unique_ptr<GLFWwindow, Deleter> window;
    std::vector<int> keysPressed;
    std::vector<int> keysReleased;
    std::vector<int> keysRepeated;
    Framebuffer defaultFramebuffer;

    static void set_window_hints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    static void initialize_glew() {
        glewExperimental = true;
        if (const auto err = glewInit(); err != GLEW_OK) {
            const std::string errString = std::string("Rendering initialization failed: ") +
                                          reinterpret_cast<const char*>(glewGetErrorString(err));
            throw std::runtime_error(errString);
        }
    }

    static void initialize_glfw() {
        if (!glfwInit()) {
            throw std::runtime_error("Rendering initialization failed");
        }
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* windowPtr = static_cast<Window*>(glfwGetWindowUserPointer(window));
        assert(windowPtr);
        if (action == GLFW_PRESS) {
            windowPtr->keysPressed.emplace_back(key);
            return;
        }
        if (action == GLFW_RELEASE) {
            windowPtr->keysReleased.emplace_back(key);
            return;
        }
        if (action == GLFW_REPEAT) {
            windowPtr->keysRepeated.emplace_back(key);
            return;
        }
    }

    void register_callbacks() { glfwSetKeyCallback(window.get(), key_callback); }
};

} // namespace tel
