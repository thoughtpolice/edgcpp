struct Shape {
  virtual int area() const = 0;
  virtual ~Shape() = default;
};

struct Square final : Shape {
  int side;

  explicit Square(int size) : side(size) {}
  int area() const override { return side * side; }
};

int checked_area(const Shape& shape) {
  int result = shape.area();
  if (result == 0) {
    throw result;
  }
  return result;
}

int main() {
  Square square(3);
  try {
    return checked_area(square) == 9 ? 0 : 1;
  } catch (int value) {
    return value + 1;
  }
}
