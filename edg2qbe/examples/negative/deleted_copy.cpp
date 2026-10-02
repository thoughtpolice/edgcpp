struct Owner { Owner() = default; Owner(const Owner&) = delete; Owner(Owner&&) = default; };
int main() { Owner original; Owner copy(original); }
