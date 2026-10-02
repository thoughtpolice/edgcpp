#include "rtti.hpp"
#include <cstdio>
#include <typeinfo>

int main() {
  using namespace rtti_abi;
  Right* right = make_diamond();
  Root* root = dynamic_cast<Root*>(right);
  Diamond* diamond = dynamic_cast<Diamond*>(root);
  Left* left = dynamic_cast<Left*>(right);
  if (!root || !diamond || !left) return 1;
  if (dynamic_cast<Right*>(left) != right || dynamic_cast<Root*>(left) != root) return 2;
  if (dynamic_cast<void*>(right) != static_cast<void*>(diamond)) return 3;
  if (root->root_value != 10 || left->left_value != 20 || right->right_value != 30) return 4;
  if (root->score() != 100 || right->score() != 100 || inspect(*root) != 100) return 5;
  if (root->identity() != root || left->identity() != left || right->identity() != right) return 6;
  if (diamond->identity() != diamond || typeid(*right) != typeid(Diamond)) return 7;
  if (dynamic_cast<Stranger*>(root) != nullptr) return 8;
  try {
    (void)dynamic_cast<Stranger&>(*root);
    return 9;
  } catch (const std::bad_cast&) {
  }
  Right* null_right = nullptr;
  if (dynamic_cast<Diamond*>(null_right) != nullptr) return 10;

  Root* stranger = make_stranger();
  if (dynamic_cast<Left*>(stranger) || dynamic_cast<Right*>(stranger)) return 11;
  if (!dynamic_cast<Stranger*>(stranger) || stranger->score() != -5) return 12;
  delete stranger;
  delete right;

  {
    Diamond local;
    Root& base = local;
    if (&dynamic_cast<Diamond&>(base) != &local || inspect(base) != 100) return 13;
    const Root& const_base = local;
    if (dynamic_cast<const Right*>(&const_base) != static_cast<const Right*>(&local)) return 14;
  }
  const Destructions counts = destructions();
  if (counts.root != 3 || counts.left != 2 || counts.right != 2 ||
      counts.diamond != 2 || counts.stranger != 1) return 15;
  std::printf("interop-rtti score=100 roots=%d diamonds=%d stranger=%d\n",
              counts.root, counts.diamond, counts.stranger);
  return 0;
}
