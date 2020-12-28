#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using Map = std::vector<std::string>;

void extendForSafety(Map &map) {
    for (auto &line : map) {
        line = '*' + line + '*';
    }
    auto margin = std::string(map.back().size(), '*');
    map.insert(map.begin(), margin);
    map.insert(map.end(), std::move(margin));
}

struct Coordinates {
    int x;
    int y;

    Coordinates &operator+=(const Coordinates &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Coordinates operator+(const Coordinates &rhs) const {
        auto n = *this;
        n += rhs;
        return n;
    }
};

auto getNeighbors(Coordinates c) {
    return std::vector<Coordinates>{{c.x - 1, c.y - 1}, {c.x, c.y - 1},
                                    {c.x + 1, c.y - 1}, {c.x - 1, c.y},
                                    {c.x + 1, c.y},     {c.x - 1, c.y + 1},
                                    {c.x, c.y + 1},     {c.x + 1, c.y + 1}};
}

auto getDirections() {
    return std::vector<Coordinates>{{-1, -1}, {0, -1},  {+1, -1}, {-1, 0},
                                    {+1, 0},  {-1, +1}, {0, +1},  {+1, +1}};
}

auto countNeighbors(const Coordinates &c, const Map &map) {
    int counter = 0;
    for (const auto &n : getNeighbors(c)) {
        if (map[n.y][n.x] == '#')
            ++counter;
    }
    return counter;
}

auto countSeenNeighbors(const Coordinates &c, const Map &map) {
    int counter = 0;
    for (const auto &direction : getDirections()) {
        for (auto pos = c + direction; map[pos.y][pos.x] != '*';
             pos += direction) {
            const auto tile = map[pos.y][pos.x];

            if (tile == '#') {
                ++counter;
                break;
            } else if (tile == 'L') {
                break;
            }
        }
    }
    return counter;
}

void transform(const Map &map, Map &next) {
    for (int y = 1; y < map.size() - 1; ++y) {
        for (int x = 1; x < map[y].size() - 1; ++x) {
            if (map[y][x] == 'L') {
                const auto neighbors = countNeighbors({x, y}, map);
                if (neighbors == 0) {
                    next[y][x] = '#';
                } else {
                    next[y][x] = map[y][x];
                }
            } else if (map[y][x] == '#') {
                const auto neighbors = countNeighbors({x, y}, map);
                if (neighbors >= 4) {
                    next[y][x] = 'L';
                } else {
                    next[y][x] = map[y][x];
                }
            }
        }
    }
}

void transformAlternative(const Map &map, Map &next) {
    for (int y = 1; y < map.size() - 1; ++y) {
        for (int x = 1; x < map[y].size() - 1; ++x) {
            if (map[y][x] == 'L') {
                const auto neighbors = countSeenNeighbors({x, y}, map);
                if (neighbors == 0) {
                    next[y][x] = '#';
                } else {
                    next[y][x] = map[y][x];
                }
            } else if (map[y][x] == '#') {
                const auto neighbors = countSeenNeighbors({x, y}, map);
                if (neighbors >= 5) {
                    next[y][x] = 'L';
                } else {
                    next[y][x] = map[y][x];
                }
            }
        }
    }
}

int countOccupied(const Map &map) {
    int counter = 0;
    for (int y = 1; y < map.size() - 1; ++y) {
        for (int x = 1; x < map[y].size() - 1; ++x) {
            if (map[y][x] == '#')
                ++counter;
        }
    }
    return counter;
}

void log(const Map &map) {
    for (const auto &l : map) {
        std::clog << l << std::endl;
    }
}

int main(int argc, char args[]) {
    Map map;
    std::string line;
    do {
        std::getline(std::cin, line);
        if (std::cin)
            map.emplace_back(line);
    } while (std::cin);

    std::clog << "Read " << map.size() << " rows" << std::endl;

    extendForSafety(map);

    const auto run = [](const auto &map, const auto &iteration,
                        const std::string &info) {
        auto current = map;
        auto next = current;

        do {
            // std::clog << "Iteration" << std::endl;

            iteration(current, next);
            std::swap(current, next);
        } while (next != current);

        std::cout << "Seats taken in " << info << ": " << countOccupied(current)
                  << std::endl;
    };

    run(map, transform, "simple rules");
    run(map, transformAlternative, "extended rules");

    return 0;
}