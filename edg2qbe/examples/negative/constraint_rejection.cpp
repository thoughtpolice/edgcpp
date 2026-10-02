template<class T> concept Incrementable = requires(T value) { ++value; };
template<Incrementable T> void increment(T& value) { ++value; }
struct Token {};
int main() { Token token; increment(token); }
