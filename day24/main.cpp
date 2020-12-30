#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Point {
    int x;
    int y;

    Point &operator+=(const Point &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

bool operator<(const Point &lhs, const Point &rhs) { return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y); }

using Delta = Point;
using Directions = std::vector<std::string>;

Point applyDirection(const Point &p, const Directions &direction) {
    auto r = p;
    for (const auto &d : direction) {
        if (d == "e") {
            r += Delta{1, 0};
        } else if (d == "w") {
            r += Delta{-1, 0};
        } else {
            const auto even = ((r.y % 2) == 0);
            int yDelta = d[0] == 'n' ? 1 : -1;
            int xDelta = 0;
            if (even) {
                xDelta = d[1] == 'e' ? 1 : 0;
            } else {
                xDelta = d[1] == 'e' ? 0 : -1;
            }
            r += Delta{xDelta, yDelta};
        }
    }
    return r;
}

std::set<Point> getCircle(const Point &p) {
    std::vector<std::string> ways = {"e", "w", "se", "sw", "ne", "nw"};
    std::set<Point> r;

    for (const auto &w : ways) {
        r.insert(applyDirection(p, {w}));
    }

    return r;
}

struct SparseHex {
    std::set<Point> black;

    void set(const Point &p) { black.insert(p); }

    bool isBlack(const Point &p) const { return black.find(p) != black.end(); }

    int countNeighbours(const Point &p) const {
        const auto circle = getCircle(p);
        int counter = 0;
        for (const auto &n : circle) {
            if (isBlack(n))
                ++counter;
        }
        return counter;
    }

    std::set<Point> getInteresting() {
        std::set<Point> r;
        for (const auto &p : black) {
            r.insert(p);

            auto circle = getCircle(p);
            r.insert(circle.begin(), circle.end());
        }

        return r;
    }
};

SparseHex transform(SparseHex in) {
    SparseHex out;

    for (const auto &p : in.getInteresting()) {
        const auto neighborCount = in.countNeighbours(p);

        if (in.isBlack(p)) {
            if (!(neighborCount == 0 || neighborCount > 2)) {
                out.set(p);
            }
        } else {
            if (neighborCount == 2) {
                out.set(p);
            }
        }
    }

    return out;
}

int main(int argc, char args[]) {
    std::vector<Directions> input;
    std::string line;
    do {
        std::getline(std::cin, line);
        if (!std::cin)
            break;

        Directions dir;
        std::size_t s = 0;
        while (s < line.size()) {
            if (line[s] == 'e') {
                dir.push_back(line.substr(s, 1));
            } else if (line[s] == 'w') {
                dir.push_back(line.substr(s, 1));
            } else {
                dir.push_back(line.substr(s, 2));
                ++s;
            }
            ++s;
        }
        input.emplace_back(std::move(dir));
    } while (std::cin);

    std::clog << "Read " << input.size() << " directions" << std::endl;

    std::map<Point, bool> state;

    for (const auto &dirs : input) {
        const auto p = applyDirection(Point{0, 0}, dirs);
        state[p] = !state[p];
    }

    SparseHex sparseHex;

    for (const auto [p, s] : state) {
        if (s) {
            sparseHex.set(p);
        }
    }

    std::cout << "Tiles turned on the black side: " << sparseHex.black.size() << std::endl;

    for (int i = 0; i < 100; ++i) {
        sparseHex = transform(sparseHex);
        std::clog << "Day " << i+1 << ": " << sparseHex.black.size() << std::endl;
    }

    return 0;
}