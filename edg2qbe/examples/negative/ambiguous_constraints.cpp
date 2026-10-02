template<class T> requires (sizeof(T) > 1) int choose(T) { return 1; }
template<class T> requires (sizeof(T) < 100) int choose(T) { return 2; }
int main() { return choose(3); }
