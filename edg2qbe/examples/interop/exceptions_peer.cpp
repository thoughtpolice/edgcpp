#include "exceptions.hpp"

namespace exception_abi {
State& state() {
  static State value{};
  return value;
}

Guard::Guard() { ++state().live_guards; }
Guard::~Guard() {
  --state().live_guards;
  ++state().destroyed_guards;
}
Anchor::Anchor(int value) : prefix(value) {}
Anchor::~Anchor() = default;
ErrorBase::ErrorBase(int value) : code(value) {}
ErrorBase::~ErrorBase() = default;
RichError::RichError(int value, int extra) : Anchor(73), ErrorBase(value), detail(extra) {
  ++state().live_errors;
}
RichError::RichError(const RichError& other)
    : Anchor(other.prefix), ErrorBase(other.code), detail(other.detail) {
  ++state().live_errors;
}
RichError::~RichError() { --state().live_errors; }

void throw_value(int code, int detail) {
  Guard producer_guard;
  throw RichError(code, detail);
}

void throw_pointer(int code, int detail) {
  Guard producer_guard;
  throw new RichError(code, detail);
}
}
