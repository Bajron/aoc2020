#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using Number = int64_t;

struct Coordinates {
    Number x;
    Number y;

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

    Coordinates operator*(Number rhs) const { return {x * rhs, y * rhs}; }
};

struct Order {
    char type;
    Number value;
};

Coordinates getDirection(char direction) {
    switch (direction) {
    case 'E':
        return Coordinates{1, 0};
    case 'W':
        return Coordinates{-1, 0};
    case 'N':
        return Coordinates{0, 1};
    case 'S':
        return Coordinates{0, -1};
    default:
        return Coordinates{0, 0};
    }
}

char rotate(char in, int direction, Number count) {
    static std::array<char, 4> compas{'E', 'S', 'W', 'N'};

    int pos = 0;
    while (compas[pos] != in)
        ++pos;

    for (Number i = 0; i < count; ++i) {
        pos += direction;
        pos += 4;
        pos %= 4;
    }

    return compas[pos];
}

Number intSin(Number deg) {
    switch (deg) {
    case 0:
        return 0;
    case 90:
        return 1;
    case 180:
        return 0;
    case 270:
        return -1;
    default:
        std::clog << deg << " does not fit integer sine" << std::endl;
        exit(1);
    }
}

Number intCos(Number deg) {
    switch (deg) {
    case 0:
        return 1;
    case 90:
        return 0;
    case 180:
        return -1;
    case 270:
        return 0;
    default:
        std::clog << deg << " does not fit integer cosine" << std::endl;
        exit(1);
    }
}

Coordinates rotate(Coordinates in, Number theta) {
    theta %= 360;
    return {in.x * intCos(theta) - in.y * intSin(theta),
            in.x * intSin(theta) + in.y * intCos(theta)};
}

struct Execution {
    Coordinates position{0, 0};
    char rotation = 'E';

    void execute(const Order &o) {
        switch (o.type) {
        case 'E':
        case 'S':
        case 'W':
        case 'N':
            position += getDirection(o.type) * o.value;
            break;
        case 'L':
            rotation = rotate(rotation, -1, o.value / 90);
            break;
        case 'R':
            rotation = rotate(rotation, 1, o.value / 90);
            break;
        case 'F':
            position += getDirection(rotation) * o.value;
            break;
        default:
            break;
        }
    }
};

struct WaypointNavigation {
    Coordinates position{0, 0};
    Coordinates waypoint{10, 1};

    void execute(const Order &o) {
        switch (o.type) {
        case 'E':
        case 'S':
        case 'W':
        case 'N':
            waypoint += getDirection(o.type) * o.value;
            break;
        case 'L':
            waypoint = rotate(waypoint, o.value);
            break;
        case 'R':
            waypoint = rotate(waypoint, 360 - o.value);
            break;
        case 'F':
            position += waypoint * o.value;
            break;
        default:
            break;
        }
    }
};

int main(int argc, char args[]) {
    std::vector<Order> input;
    do {
        Order o;
        std::cin >> o.type >> o.value;
        if (std::cin)
            input.emplace_back(std::move(o));
    } while (std::cin);

    std::clog << "Read " << input.size() << " orders" << std::endl;

    Execution ex;
    for (const auto &o : input) {
        ex.execute(o);
    }

    std::cout << "Guess orders distance: "
              << std::abs(ex.position.x) + std::abs(ex.position.y) << std::endl;

    WaypointNavigation wn;
    for (const auto &o : input) {
        wn.execute(o);
    }
    std::cout << "Instruction orders distance: "
              << std::abs(wn.position.x) + std::abs(wn.position.y) << std::endl;
    return 0;
}