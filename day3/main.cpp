#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

struct Vector {
    int x;
    int y;

    Vector &operator+=(const Vector &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

struct Map {
    std::vector<std::string> lines;

    int getHeight() const { return static_cast<int>(lines.size()); }

    bool isTree(const Vector &position) const {
        return lines[position.y][position.x] == '#';
    }

    void advance(Vector &position, const Vector &change) const {
        position += change;
        position.x %= static_cast<int>(lines.front().size());
    }
};

int traverseCounting(const Map &map, const Vector &slope) {
    Vector position{0, 0};
    int treeCount = 0;

    while (position.y < map.getHeight()) {
        if (map.isTree(position))
            ++treeCount;

        map.advance(position, slope);
    }

    return treeCount;
}

int main(int argc, char args[]) {
    Map input;
    std::string buffer;

    do {
        std::getline(std::cin, buffer);
        if (std::cin)
            input.lines.emplace_back(std::move(buffer));
    } while (std::cin);

    std::cout << "Tree count: " << traverseCounting(input, Vector{3, 1})
              << std::endl;

    const auto slopes =
        std::vector<Vector>{{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
    std::vector<std::int64_t> counts;
    for (const auto &s : slopes) {
        const auto c = traverseCounting(input, s);
        std::clog << "Trees on {" << s.x << ", " << s.y << "}: " << c
                  << std::endl;
        counts.push_back(c);
    }

    std::cout << "Counts product: "
              << std::accumulate(counts.begin(), counts.end(), std::int64_t(1),
                                 std::multiplies<std::int64_t>());

    return 0;
}