#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

int main() {
  std::vector<int> values{4, 1, 3, 2, 3, 1};
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  const int sum = std::accumulate(values.begin(), values.end(), 0);

  std::map<std::string, int> counts;
  for (const std::string& word : {std::string("edg"), std::string("gcc"),
                                std::string("edg")}) {
    ++counts[word];
  }

  std::cout << "sorted=";
  for (std::size_t i = 0; i < values.size(); ++i) {
    std::cout << (i == 0 ? "" : ",") << values[i];
  }
  std::cout << " sum=" << sum << " edg=" << counts.at("edg")
            << " gcc=" << counts.at("gcc") << '\n';
  return values == std::vector<int>({1, 2, 3, 4}) && sum == 10 &&
                 counts.at("edg") == 2 && counts.at("gcc") == 1
             ? 0 : 1;
}
