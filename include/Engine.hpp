#pragma once
#include "InputManager.hpp"
#include "Rendering.hpp"
#include "Scene.hpp"
#include <filesystem>
#include <sol/sol.hpp>

namespace tel {
class Engine {
  public:
    Engine()
        : window(std::make_unique<Window>(800, 600, "Test")), rendering(std::make_unique<Rendering>(window.get())),
          inputManager(std::make_unique<InputManager>(window.get())) {}

    Rendering& rendering_system() { return *rendering; }

    Scene& current_scene() { return currentScene; }

    void start_main_loop() {
        assert(ready_to_start());
        while (!window->should_close()) {
            window->poll_events();
            inputManager->handle_input();
            rendering->render_scene(currentScene);
            window->swap_buffers();
        }
    }

    [[nodiscard]] bool ready_to_start() const {
        return rendering != nullptr && inputManager != nullptr && window != nullptr;
    }

  private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Rendering> rendering;
    std::unique_ptr<InputManager> inputManager;
    Scene currentScene{.camera = Camera::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f)};
};
} // namespace tel
