constexpr int invalid_index() { int values[]{1, 2}; return values[2]; }
constexpr int value = invalid_index();
int main() { return value; }
