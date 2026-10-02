#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

struct Resource {
  int& live;
  explicit Resource(int& count) : live(count) { ++live; }
  virtual ~Resource() { --live; }
  virtual std::string name() const { return "resource"; }
};

struct NamedResource final : Resource {
  using Resource::Resource;
  std::string name() const override { return "edg"; }
};

int main() {
  int live = 0;
  std::string message;
  try {
    std::unique_ptr<Resource> resource = std::make_unique<NamedResource>(live);
    if (live != 1 || resource->name() != "edg") {
      return 1;
    }
    throw std::runtime_error(resource->name() + ": unwind");
  } catch (const std::exception& error) {
    message = error.what();
  }

  auto value = std::make_shared<int>(42);
  std::weak_ptr<int> observer = value;
  const int answer = *observer.lock();
  value.reset();
  std::cout << message << " live=" << live << " answer=" << answer
            << " expired=" << observer.expired() << '\n';
  return message == "edg: unwind" && live == 0 && answer == 42 &&
                 observer.expired()
             ? 0 : 1;
}
