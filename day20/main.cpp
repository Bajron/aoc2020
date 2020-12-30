#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Tile = std::vector<std::string>;
using Number = std::int64_t;

// Rotations and flipping:
// DC CB BA AD // 4 rotations
// AB DA CD BC
//
// CD DA AB BC // 4 rotations after flip
// BA CB DC AD

// sides up -> down
// rows left -> right

Tile rotateLeft(const Tile &tile) {
    Tile n = tile;
    const auto w = tile.size();
    const auto h = tile.front().size();
    assert(w == h);
    for (std::size_t y = 0; y < tile.size(); ++y) {
        for (std::size_t x = 0; x < tile[y].size(); ++x) {
            n[w - 1 - x][y] = tile[y][x];
        }
    }
    return n;
}

Tile flip(const Tile &tile) {
    Tile n = tile;
    for (auto &row : n) {
        std::reverse(row.begin(), row.end());
    }
    return n;
}

struct TileEdges {
    TileEdges() = default;
    TileEdges(const Tile &t) : top{t.front()}, bottom{t.back()} {
        for (const auto &line : t) {
            left += line.front();
            right += line.back();
        }
        std::clog << "xx: " << top << "|" << bottom << "|" << left << "|" << right << std::endl;
        assert(top.size() == bottom.size());
        assert(top.size() == left.size());
        assert(bottom.size() == right.size());
    }
    std::string top, bottom, left, right;

    TileEdges rotatedLeft() const {
        TileEdges n;
        n.top = right;
        n.bottom = left;
        n.left = std::string{top.rbegin(), top.rend()};
        n.right = std::string{bottom.rbegin(), bottom.rend()};
        return n;
    }

    // Over vertical axis
    TileEdges flipped() const {
        TileEdges n;
        n.top = std::string{top.rbegin(), top.rend()};
        n.bottom = std::string{bottom.rbegin(), bottom.rend()};
        n.left = right;
        n.right = left;
        return n;
    }
};

using TransformationSet = std::vector<TileEdges>;
using FlattenedConnections = std::set<std::string>;

TransformationSet generateTranformationSet(const TileEdges &prototype) {
    TransformationSet r;
    r.reserve(8);
    r.push_back(prototype);
    for (int i = 0; i < 3; ++i)
        r.push_back(r.back().rotatedLeft());
    for (int i = 0; i < 4; ++i)
        r.push_back(r[i].flipped());
    assert(r.size() == 8);
    return r;
}

Tile getVariant(const Tile &tile, int variant) {
    auto r = tile;
    for (int i = 0; i < variant % 4; ++i) {
        r = rotateLeft(r);
    }
    if (variant >= 4) {
        r = flip(r);
    }
    return r;
}

FlattenedConnections getFlattenedConnections(const TransformationSet &ts) {
    FlattenedConnections r;
    for (const auto &t : ts) {
        r.insert(t.top);
        r.insert(t.bottom);
        r.insert(t.left);
        r.insert(t.right);
    }
    return r;
}

struct Node {
    explicit Node(Number t) : tile{t} {}

    Number tile;
    int variant = -1;

    Node *left = nullptr;
    Node *right = nullptr;
    Node *top = nullptr;
    Node *bottom = nullptr;
};

int main(int argc, char args[]) {
    std::string line;
    Number number;

    std::unordered_map<Number, Tile> input;

    do {
        std::cin >> line; // skip "Tile"
        if (!std::cin || line.empty())
            break;
        std::cin >> number;
        std::getline(std::cin, line);

        Tile tile;
        do {
            std::getline(std::cin, line);
            if (!line.empty() && std::cin)
                tile.push_back(line);
        } while (!line.empty() && std::cin);
        input[number] = std::move(tile);
    } while (std::cin);

    std::clog << "Read " << input.size() << " tiles" << std::endl;

    std::unordered_map<Number, TileEdges> edges;
    std::unordered_map<Number, TransformationSet> transformations;
    std::unordered_map<Number, FlattenedConnections> connections;

    for (const auto &[n, tile] : input) {
        edges[n] = TileEdges{tile};
        transformations[n] = generateTranformationSet(edges[n]);
        connections[n] = getFlattenedConnections(transformations[n]);
    }

    std::unordered_map<Number, std::unordered_set<Number>> mayConnectTo;
    for (const auto &[n1, connections1] : connections) {
        for (const auto &[n2, connections2] : connections) {
            if (n1 == n2)
                continue;

            std::vector<std::string> common;
            std::set_intersection(connections1.begin(), connections1.end(), connections2.begin(), connections2.end(),
                                  std::back_inserter(common));

            if (!common.empty()) {
                mayConnectTo[n1].insert(n2);
            }
        }
    }
    std::clog << "Connections: " << std::endl;

    Number cornerProduct = 1;
    std::vector<Number> corners;

    for (const auto &[n, conns] : mayConnectTo) {
        std::clog << n << ": ";
        for (const auto &c : conns) {
            std::clog << c << ", ";
        }
        std::clog << std::endl;

        if (conns.size() == 2) {
            corners.push_back(n);
            cornerProduct *= n;
        }
    }

    std::cout << "Found " << corners.size() << " possible corners, checksum: " << cornerProduct << std::endl;

    // Now here we assume single good match as observed
    auto neighborsToOrient = mayConnectTo;

    std::unordered_map<Number, std::unique_ptr<Node>> keeper;
    for (const auto &[n, _] : input) {
        keeper.insert(std::make_pair(n, std::make_unique<Node>(n)));
    }
    const auto getNode = [&keeper](Number n) { return keeper.at(n).get(); };

    // Orientation does not matter as we may flip it and rotate it anyway (like
    // glass puzzles)

    auto start = corners[0];
    auto upperLeft = keeper.at(start).get();
    auto first = mayConnectTo[start].begin();

    upperLeft->right = getNode(*first);
    upperLeft->right->left = upperLeft;
    neighborsToOrient[start].erase(*first);
    neighborsToOrient[*first].erase(start);

    // fill right
    auto current = upperLeft->right;
    while (current) {
        auto neighbors = neighborsToOrient[current->tile];

        if (neighbors.size() > 1) {
            auto onRight = current->tile;
            for (const auto n : neighbors) {
                if (neighborsToOrient[n].size() <= 3) {
                    assert(onRight == current->tile);
                    onRight = n;
                }
            }
            assert(onRight != current->tile);

            current->right = getNode(onRight);
            current->right->left = current;
            neighborsToOrient[current->tile].erase(current->right->tile);
            neighborsToOrient[current->right->tile].erase(current->tile);
        }

        assert(neighborsToOrient[current->tile].size() == 1);

        current->bottom = getNode(*neighborsToOrient[current->tile].begin());
        current->bottom->top = current;
        neighborsToOrient[current->tile].erase(current->bottom->tile);
        neighborsToOrient[current->bottom->tile].erase(current->tile);

        current = current->right;
    }

    ++first;
    upperLeft->bottom = getNode(*first);
    upperLeft->bottom->top = upperLeft;
    neighborsToOrient[start].erase(*first);
    neighborsToOrient[*first].erase(start);

    // fill down
    current = upperLeft->bottom;
    while (current) {
        auto neighbors = neighborsToOrient[current->tile];

        if (neighbors.size() > 1) {
            auto onBottom = current->tile;
            for (const auto n : neighbors) {
                if (neighborsToOrient[n].size() <= 3 && !getNode(n)->top) {
                    assert(onBottom == current->tile);
                    onBottom = n;
                }
            }
            assert(onBottom != current->tile);

            current->bottom = getNode(onBottom);
            current->bottom->top = current;
            neighborsToOrient[current->tile].erase(current->bottom->tile);
            neighborsToOrient[current->bottom->tile].erase(current->tile);
        }

        assert(neighborsToOrient[current->tile].size() == 1);

        current->right = getNode(*neighborsToOrient[current->tile].begin());
        current->right->left = current;
        neighborsToOrient[current->tile].erase(current->right->tile);
        neighborsToOrient[current->right->tile].erase(current->tile);

        current = current->bottom;
    }

    // fill all rows right
    auto currentStart = upperLeft->right->bottom;
    while (currentStart) {
        current = currentStart;
        while (current) {
            auto neighbors = neighborsToOrient[current->tile];

            auto onRight = current->tile;
            auto onBottom = current->tile;
            for (const auto n : neighbors) {
                if (getNode(n)->top) {
                    assert(onRight == current->tile);
                    onRight = n;
                } else {
                    assert(onBottom == current->tile);
                    onBottom = n;
                }
            }

            if (onRight != current->tile) {
                current->right = getNode(onRight);
                current->right->left = current;
                neighborsToOrient[current->tile].erase(current->right->tile);
                neighborsToOrient[current->right->tile].erase(current->tile);
            }

            if (onBottom != current->tile) {
                current->bottom = getNode(*neighborsToOrient[current->tile].begin());
                current->bottom->top = current;
                neighborsToOrient[current->tile].erase(current->bottom->tile);
                neighborsToOrient[current->bottom->tile].erase(current->tile);
            }
            current = current->right;
        }
        currentStart = currentStart->bottom;
    }

    for (const auto &[n, nto] : neighborsToOrient) {
        std::clog << "to orient for " << n << ": " << nto.size() << std::endl;
    }

    // I hope single orientation matches, right?..
    {
        auto onTopLeft = upperLeft;
        auto onRight = upperLeft->right;
        auto onBottom = upperLeft->bottom;

        for (int vl = 0; vl < transformations[onTopLeft->tile].size(); ++vl) {
            for (int vr = 0; vr < transformations[onRight->tile].size(); ++vr) {
                for (int vb = 0; vb < transformations[onBottom->tile].size(); ++vb) {
                    const auto &l = transformations[onTopLeft->tile][vl];
                    const auto &r = transformations[onRight->tile][vr];
                    const auto &b = transformations[onBottom->tile][vb];

                    if (l.right == r.left && l.bottom == b.top) {
                        onTopLeft->variant = vl;
                        onRight->variant = vr;
                        onBottom->variant = vb;
                        std::clog << "Found variant for upper left corner " << vl << " " << vr << " " << vb
                                  << std::endl;
                    }
                }
            }
        }
    }

    current = upperLeft->right->right;
    while (current) {
        for (int v = 0; v < transformations[current->tile].size(); ++v) {
            const auto l = transformations[current->left->tile][current->left->variant].right;
            const auto r = transformations[current->tile][v].left;
            if (l == r) {
                current->variant = v;
                std::clog << "Found variant for " << current->tile << ": " << v << std::endl;
            }
        }
        current = current->right;
    }

    current = upperLeft->bottom->bottom;
    while (current) {
        for (int v = 0; v < transformations[current->tile].size(); ++v) {
            const auto t = transformations[current->top->tile][current->top->variant].bottom;
            const auto b = transformations[current->tile][v].top;
            if (t == b) {
                current->variant = v;
                std::clog << "Found variant for " << current->tile << ": " << v << std::endl;
            }
        }
        current = current->bottom;
    }

    // TODO this can probably reduce one of the loops above
    currentStart = upperLeft->right->bottom;
    while (currentStart) {
        current = currentStart;
        while (current) {
            for (int v = 0; v < transformations[current->tile].size(); ++v) {
                const auto l = transformations[current->left->tile][current->left->variant].right;
                const auto r = transformations[current->tile][v].left;
                if (l == r) {
                    current->variant = v;
                    std::clog << "Found variant for " << current->tile << ": " << v << std::endl;
                }
            }
            current = current->right;
        }
        currentStart = currentStart->bottom;
    }

    int width = 0;
    current = upperLeft;
    while (current) {
        ++width;
        current = current->right;
    };

    int height = 0;
    current = upperLeft;
    while (current) {
        ++height;
        current = current->bottom;
    };

    auto tileEdge = input.begin()->second.size();

    std::clog << "Width: " << width << " height: " << height << " tileEdge: " << tileEdge << std::endl;

    std::vector<std::string> buffer(height * tileEdge, std::string(width * tileEdge, '?'));

    {
        int y = 0;
        currentStart = upperLeft;
        while (currentStart) {
            int x = 0;
            current = currentStart;
            while (current) {
                auto t = getVariant(input[current->tile], current->variant);
                for (int yy = 0; yy < t.size(); ++yy) {
                    for (int xx = 0; xx < t.front().size(); ++xx) {
                        buffer[y * tileEdge + yy][x * tileEdge + xx] = t[yy][xx];
                    }
                }
                current = current->right;
                ++x;
            }
            currentStart = currentStart->bottom;
            ++y;
        }
    }

    if (buffer.front().size() < 80) {
        for (const auto &line : buffer) {
            std::clog << line << std::endl;
        }
    }

    buffer.resize(height * (tileEdge - 2));
    for (auto &l : buffer) {
        l.resize(0);
        l.resize(width * (tileEdge - 2), '?');
    }

    {
        int y = 0;
        currentStart = upperLeft;
        while (currentStart) {
            int x = 0;
            current = currentStart;
            while (current) {
                auto t = getVariant(input[current->tile], current->variant);
                for (int yy = 1; yy < t.size() - 1; ++yy) {
                    for (int xx = 1; xx < t.front().size() - 1; ++xx) {
                        buffer[y * (tileEdge - 2) + yy - 1][x * (tileEdge - 2) + xx - 1] = t[yy][xx];
                    }
                }
                current = current->right;
                ++x;
            }
            currentStart = currentStart->bottom;
            ++y;
        }
    }
    if (buffer.front().size() < 80) {
        for (const auto &line : buffer) {
            std::clog << line << std::endl;
        }
    }

    const auto monster = std::vector<std::string>{{
        "                  # ",
        "#    ##    ##    ###",
        " #  #  #  #  #  #   ",
    }};

    bool hadMonster = false;
    int trial = 0;
    while (!hadMonster) {
        for (std::size_t y = 0; y < buffer.size() - monster.size(); ++y) {
            for (std::size_t x = 0; x < buffer.front().size() - monster.front().size(); ++x) {

                bool isMonster = true;
                for (std::size_t my = 0; my < monster.size() && isMonster; ++my) {
                    for (std::size_t mx = 0; mx < monster.front().size() && isMonster; ++mx) {
                        if (monster[my][mx] != '#')
                            continue;

                        if (buffer[y + my][x + mx] == '.') {
                            isMonster = false;
                            break;
                        }
                    }
                }
                if (isMonster) {
                    hadMonster = true;
                    for (std::size_t my = 0; my < monster.size(); ++my) {
                        for (std::size_t mx = 0; mx < monster.front().size(); ++mx) {
                            if (monster[my][mx] != '#')
                                continue;

                            buffer[y + my][x + mx] = 'O';
                        }
                    }
                }
            }
        }
        if (!hadMonster) {
            if (trial % 2 == 0) {
                buffer = flip(buffer);
            } else {
                buffer = rotateLeft(flip(buffer));
            }
            ++trial;
        }
    }
    int nonMonsterPounds = 0;
    for (std::size_t y = 0; y < buffer.size(); ++y) {
        for (std::size_t x = 0; x < buffer.front().size(); ++x) {
            if (buffer[y][x] == '#')
                ++nonMonsterPounds;
        }
    }

    std::cout << "And all this for " << nonMonsterPounds << std::endl;

    return 0;
}