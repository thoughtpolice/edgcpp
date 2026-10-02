class Secret { Secret() = default; };
int main() { auto construct = []<class T>() { return T{}; }; construct.operator()<Secret>(); }
