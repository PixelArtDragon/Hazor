#pragma once
#include <cassert>
#include <utility>

namespace tel {
enum class EngagedMoveAssignBehavior { Assert, Discard };

template <typename T, T defaultState = T{},
          EngagedMoveAssignBehavior moveAssignBehavior = EngagedMoveAssignBehavior::Discard>
class Moving {
  public:
    Moving() : val(defaultState) {}

    template <typename... Args>
    Moving(Args&&... args) : val(std::forward<Args>(args)...) {}

    Moving(const Moving&) = delete;

    Moving& operator=(const Moving&) = delete;

    Moving(Moving&& other) noexcept(std::is_nothrow_copy_constructible_v<T>) : val(other.val) {
        other.val = defaultState;
    }

    Moving& operator=(Moving&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
        if (this == &other) {
            return *this;
        }
        if constexpr (moveAssignBehavior == EngagedMoveAssignBehavior::Assert) {
            assert(val == defaultState);
        }
        this->val = std::move(other.val);
        other.val = defaultState;
        return *this;
    }

    operator T&() { return val; }

    operator const T&() const { return val; }

    const T& value() const { return val; }

    T& value() { return val; }

    ~Moving() = default;

  private:
    T val;
};
} // namespace tel
