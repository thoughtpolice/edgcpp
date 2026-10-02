#include "exceptions.hpp"
#include <cstdio>
#include <typeinfo>

namespace {
using namespace exception_abi;

void relay_value(int& checked) {
  Guard consumer_guard;
  try {
    throw_value(91, 17);
  } catch (ErrorBase& base) {
    RichError* complete = dynamic_cast<RichError*>(&base);
    if (!complete || complete->prefix != 73 || complete->detail != 17 || base.code != 91) {
      checked = 1;
    } else if (static_cast<void*>(&base) == static_cast<void*>(complete)) {
      checked = 2;
    } else if (typeid(base) != typeid(RichError)) {
      checked = 3;
    } else if (state().live_guards != 1 || state().destroyed_guards != 1 ||
               state().live_errors < 1) {
      checked = 4;
    } else {
      checked = 100;
    }
    throw;
  }
}

void relay_pointer(int& checked) {
  Guard consumer_guard;
  try {
    throw_pointer(42, 8);
  } catch (const ErrorBase* base) {
    const RichError* complete = dynamic_cast<const RichError*>(base);
    if (!complete || complete->prefix != 73 || complete->detail != 8 || base->code != 42) {
      checked = 5;
    } else if (static_cast<const void*>(base) == static_cast<const void*>(complete)) {
      checked = 6;
    } else if (state().live_guards != 1 || state().destroyed_guards != 3 ||
               state().live_errors != 1) {
      checked = 7;
    } else {
      checked = 200;
    }
    throw;
  }
}
}

int main() {
  using namespace exception_abi;
  int checked = 0;
  int sum = 0;
  try {
    relay_value(checked);
    return 10;
  } catch (const RichError& error) {
    if (checked != 100) return 11;
    if (error.prefix != 73 || error.code != 91 || error.detail != 17) return 12;
    if (state().live_guards != 0 || state().destroyed_guards != 2) return 13;
    sum += error.code + error.detail;
  } catch (...) {
    return 14;
  }
  if (state().live_errors != 0) return 15;

  try {
    relay_pointer(checked);
    return 16;
  } catch (RichError* error) {
    // Rethrow retains RichError*, not the adjusted const ErrorBase* catch type.
    if (checked != 200) {
      delete error;
      return 17;
    }
    if (error->prefix != 73 || error->code != 42 || error->detail != 8) {
      delete error;
      return 18;
    }
    sum += error->code + error->detail;
    delete error;
  } catch (...) {
    return 19;
  }
  if (state().live_errors != 0 || state().live_guards != 0 ||
      state().destroyed_guards != 4 || sum != 158) return 20;
  std::printf("interop-exceptions sum=%d guards=%d live=%d\n",
              sum, state().destroyed_guards, state().live_errors);
  return 0;
}
