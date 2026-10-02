#ifndef INTEROP_RTTI_HPP
#define INTEROP_RTTI_HPP

namespace rtti_abi {

struct Destructions {
  int root;
  int left;
  int right;
  int diamond;
  int stranger;
};

struct Root {
  int root_value;
  explicit Root(int value);
  virtual ~Root();
  virtual int score() const = 0;
  virtual Root* identity();
};

struct Left : virtual Root {
  int left_value;
  explicit Left(int value);
  ~Left() override;
  Left* identity() override;
};

struct Right : virtual Root {
  int right_value;
  explicit Right(int value);
  ~Right() override;
  Right* identity() override;
};

struct Diamond final : Left, Right {
  int own_value;
  Diamond();
  ~Diamond() override;
  int score() const override;
  Diamond* identity() override;
};

struct Stranger final : Root {
  Stranger();
  ~Stranger() override;
  int score() const override;
};

Right* make_diamond();
Root* make_stranger();
int inspect(const Root& object);
Destructions destructions();

}

#endif
