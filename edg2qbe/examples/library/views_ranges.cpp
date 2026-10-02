#include <algorithm>
#include <cstdio>
#include <ranges>
#include <span>
#include <string_view>

struct Record {
  int id;
  int score;
};

int main() {
  Record records[] = {{0, 100}, {1, 8}, {2, 3}, {3, 6}, {4, 200}};
  std::span<Record> all(records);
  auto middle = all.subspan(1, 3);
  if (middle.size() != 3 || middle.data() != records + 1)
    return 1;
  middle.front().score = 14;
  if (records[1].score != 14)
    return 2;
  std::ranges::sort(middle, {}, &Record::score);
  if (middle[0].id != 2 || middle[1].id != 3 || middle[2].id != 1 ||
      all.front().id != 0 || all.front().score != 100 || all.back().id != 4 ||
      all.back().score != 200)
    return 3;
  auto found = std::ranges::find(middle, 3, &Record::id);
  if (found == middle.end() || &*found != records + 2 || found->score != 6)
    return 4;

  char text[] = "prefix:delta:suffix";
  const std::string_view whole(text);
  const auto word = whole.substr(7, 5);
  if (word != "delta" || word.data() != text + 7)
    return 5;
  text[8] = 'E';
  if (word != "dElta" || whole.substr(7, 5) != word)
    return 6;
  const auto empty_text = whole.substr(whole.size());
  const auto empty_records = all.subspan(all.size());
  if (!empty_text.empty() || !empty_records.empty() ||
      std::ranges::distance(empty_records) != 0)
    return 7;

  int evaluations = 0;
  auto pipeline = middle |
                  std::views::filter([](const Record& item) { return item.score >= 6; }) |
                  std::views::transform([&](const Record& item) {
                    ++evaluations;
                    return item.id * 10 + item.score;
                  });
  if (evaluations != 0)
    return 8;
  auto current = pipeline.begin();
  if (current == pipeline.end() || evaluations != 0)
    return 9;
  const int before_mutation = *current;
  if (before_mutation != 36 || evaluations != 1)
    return 10;
  middle[1].score = 9;
  const int after_mutation = *current;
  if (after_mutation != 39 || evaluations != 2)
    return 11;
  ++current;
  if (current == pipeline.end() || evaluations != 2)
    return 12;
  const int last = *current;
  if (last != 24 || evaluations != 3)
    return 13;
  ++current;
  if (current != pipeline.end() || evaluations != 3)
    return 14;
  auto empty_pipeline = empty_records |
                        std::views::transform([&](const Record& item) {
                          ++evaluations;
                          return item.score;
                        });
  if (empty_pipeline.begin() != empty_pipeline.end() || evaluations != 3)
    return 15;

  std::printf("views=%zu sorted=%d,%d,%d pipeline=%d evaluations=%d text=%.*s empty=1\n",
              middle.size(), middle[0].id, middle[1].id, middle[2].id,
              after_mutation + last, evaluations, static_cast<int>(word.size()),
              word.data());
}
