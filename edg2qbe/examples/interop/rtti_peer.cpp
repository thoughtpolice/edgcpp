#include "rtti.hpp"
#include <typeinfo>

namespace rtti_abi {
namespace {
Destructions counts{};
}

Root::Root(int value) : root_value(value) {}
Root::~Root() { ++counts.root; }
Root* Root::identity() { return this; }
Left::Left(int value) : Root(0), left_value(value) {}
Left::~Left() { ++counts.left; }
Left* Left::identity() { return this; }
Right::Right(int value) : Root(0), right_value(value) {}
Right::~Right() { ++counts.right; }
Right* Right::identity() { return this; }
Diamond::Diamond() : Root(10), Left(20), Right(30), own_value(40) {}
Diamond::~Diamond() { ++counts.diamond; }
int Diamond::score() const {
  return root_value + left_value + right_value + own_value;
}
Diamond* Diamond::identity() { return this; }
Stranger::Stranger() : Root(5) {}
Stranger::~Stranger() { ++counts.stranger; }
int Stranger::score() const { return -root_value; }

Right* make_diamond() { return new Diamond; }
Root* make_stranger() { return new Stranger; }

int inspect(const Root& object) {
  const Diamond* diamond = dynamic_cast<const Diamond*>(&object);
  if (!diamond || typeid(object) != typeid(Diamond)) return -1;
  const Left* left = dynamic_cast<const Left*>(&object);
  const Right* right = dynamic_cast<const Right*>(&object);
  if (!left || !right || dynamic_cast<const Right*>(left) != right) return -2;
  if (dynamic_cast<const void*>(right) != static_cast<const void*>(diamond)) return -3;
  return object.score();
}

Destructions destructions() { return counts; }
}
