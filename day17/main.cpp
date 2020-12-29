#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Point {
    int x, y, z;

    bool operator==(const Point &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
};
namespace std {
template <> struct hash<Point> {
    std::size_t operator()(const Point &point) const noexcept {
        std::size_t h = std::hash<int>{}(point.z);
        h ^= (std::hash<int>{}(point.y) << 1);
        h ^= (std::hash<int>{}(point.x) << 1);
        return h;
    }
};
} // namespace std

std::unordered_set<Point> getCube(const Point &p) {
    std::unordered_set<Point> r;
    for (int z = p.z - 1; z <= p.z + 1; ++z) {
        for (int y = p.y - 1; y <= p.y + 1; ++y) {
            for (int x = p.x - 1; x <= p.x + 1; ++x) {
                r.emplace(Point{x, y, z});
            }
        }
    }
    return r;
}

struct Point4 {
    int x, y, z, w;

    bool operator==(const Point4 &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
};
namespace std {
template <> struct hash<Point4> {
    std::size_t operator()(const Point4 &point) const noexcept {
        std::size_t h = std::hash<int>{}(point.w);
        h ^= (std::hash<int>{}(point.z) << 1);
        h ^= (std::hash<int>{}(point.y) << 1);
        h ^= (std::hash<int>{}(point.x) << 1);
        return h;
    }
};
} // namespace std

std::unordered_set<Point4> getCube(const Point4 &p) {
    std::unordered_set<Point4> r;
    for (int w = p.w - 1; w <= p.w + 1; ++w) {
        for (int z = p.z - 1; z <= p.z + 1; ++z) {
            for (int y = p.y - 1; y <= p.y + 1; ++y) {
                for (int x = p.x - 1; x <= p.x + 1; ++x) {
                    r.emplace(Point4{x, y, z, w});
                }
            }
        }
    }
    return r;
}

struct SparseCube {
    std::unordered_map<int, std::unordered_map<int, std::unordered_set<int>>>
        storage;

    void set(const Point &p) { set(p.z, p.y, p.x); }
    void set(int z, int y, int x) { storage[z][y].insert(x); }

    bool isSet(const Point &p) const { return isSet(p.z, p.y, p.x); }
    bool isSet(int z, int y, int x) const {
        const auto zz = storage.find(z);
        if (zz == storage.end())
            return false;
        const auto yy = zz->second.find(y);
        if (yy == zz->second.end())
            return false;
        return yy->second.find(x) != yy->second.end();
    }

    std::size_t countActive() const {
        std::size_t counter = 0;
        for (const auto &[_, yStorage] : storage) {
            for (const auto &[_, xStorage] : yStorage) {
                counter += xStorage.size();
            }
        }
        return counter;
    }

    std::unordered_set<Point> getBoundaries() const {
        std::unordered_set<Point> r;
        for (const auto &[z, yStorage] : storage) {
            for (const auto &[y, xStorage] : yStorage) {
                for (auto x : xStorage) {
                    for (auto &&p : getCube(Point{x, y, z})) {
                        r.emplace(p);
                    }
                }
            }
        }
        return r;
    }

    int countNeighbors(const Point &p) const {
        auto cube = getCube(p);
        cube.erase(p);

        int counter = 0;
        for (const auto &n : cube) {
            if (isSet(n))
                ++counter;
        }
        return counter;
    }
};

struct SparseCube4 {
    std::unordered_map<
        int, std::unordered_map<
                 int, std::unordered_map<int, std::unordered_set<int>>>>
        storage;

    void set(const Point4 &p) { set(p.w, p.z, p.y, p.x); }
    void set(int w, int z, int y, int x) { storage[w][z][y].insert(x); }

    bool isSet(const Point4 &p) const { return isSet(p.w, p.z, p.y, p.x); }
    bool isSet(int w, int z, int y, int x) const {
        const auto ww = storage.find(w);
        if (ww == storage.end())
            return false;
        const auto zz = ww->second.find(z);
        if (zz == ww->second.end())
            return false;
        const auto yy = zz->second.find(y);
        if (yy == zz->second.end())
            return false;
        return yy->second.find(x) != yy->second.end();
    }

    std::size_t countActive() const {
        std::size_t counter = 0;
        for (const auto &[_, zStorage] : storage) {
            for (const auto &[_, yStorage] : zStorage) {
                for (const auto &[_, xStorage] : yStorage) {
                    counter += xStorage.size();
                }
            }
        }
        return counter;
    }

    std::unordered_set<Point4> getBoundaries() const {
        std::unordered_set<Point4> r;
        for (const auto &[w, zStorage] : storage) {
            for (const auto &[z, yStorage] : zStorage) {
                for (const auto &[y, xStorage] : yStorage) {
                    for (auto x : xStorage) {
                        for (auto &&p : getCube(Point4{x, y, z, w})) {
                            r.emplace(p);
                        }
                    }
                }
            }
        }
        return r;
    }

    int countNeighbors(const Point4 &p) const {
        auto cube = getCube(p);
        cube.erase(p);

        int counter = 0;
        for (const auto &n : cube) {
            if (isSet(n))
                ++counter;
        }
        return counter;
    }
};

template <class CubeType> CubeType transform(const CubeType &input) {
    CubeType output;

    for (const auto &point : input.getBoundaries()) {
        const auto n = input.countNeighbors(point);
        if (input.isSet(point)) {
            if (n == 2 || n == 3) {
                output.set(point);
            }
        } else {
            if (n == 3) {
                output.set(point);
            }
        }
    }

    return output;
}

int main(int argc, char args[]) {
    std::string line;
    std::vector<std::string> initial;

    do {
        std::getline(std::cin, line);
        if (std::cin)
            initial.emplace_back(std::move(line));
    } while (std::cin);

    const int h = static_cast<int>(initial.size());
    const int w = static_cast<int>(initial.front().size());

    SparseCube current;
    SparseCube4 currentExtended;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (initial[y][x] == '#') {
                current.set(0, y, x);
                currentExtended.set(0, 0, y, x);
            }
        }
    }

    for (int i = 0; i < 6; ++i) {
        current = transform(current);
    }
    std::cout << "3d: Active after 6 rounds: " << current.countActive()
              << std::endl;

    for (int i = 0; i < 6; ++i) {
        currentExtended = transform(currentExtended);
    }
    std::cout << "4d: Active after 6 rounds: " << currentExtended.countActive()
              << std::endl;

    return 0;
}