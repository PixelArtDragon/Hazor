#pragma once
#include "Window.hpp"

namespace tel {

class InputManager {
  public:
    explicit InputManager(Window* window) : window(window) {}

    void handle_input() {
        // TODO: Add input handling
    }

  private:
    Window* window;
};
} // namespace tel
