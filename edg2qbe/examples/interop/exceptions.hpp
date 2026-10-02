#ifndef INTEROP_EXCEPTIONS_HPP
#define INTEROP_EXCEPTIONS_HPP

namespace exception_abi {

struct State {
  int live_errors;
  int live_guards;
  int destroyed_guards;
};
State& state();

struct Guard {
  Guard();
  ~Guard();
  Guard(const Guard&) = delete;
  Guard& operator=(const Guard&) = delete;
};

struct Anchor {
  int prefix;
  explicit Anchor(int value);
  virtual ~Anchor();
};

struct ErrorBase {
  int code;
  explicit ErrorBase(int value);
  virtual ~ErrorBase();
};

struct RichError final : Anchor, ErrorBase {
  int detail;
  RichError(int code, int detail);
  RichError(const RichError& other);
  ~RichError() override;
};

void throw_value(int code, int detail);
void throw_pointer(int code, int detail);

}

#endif
