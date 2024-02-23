#pragma once

namespace tel {
template <typename T>
class MustInit {
  public:
    explicit(false) MustInit(const T& value) : underlyingValue(value) {}

    const T& value() const { return underlyingValue; }

    explicit(false) operator const T&() const { return underlyingValue; }

  private:
    T underlyingValue;
};
} // namespace tel