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

using Number = int64_t;

int main(int argc, char args[]) {
    Number card, door;
    std::cin >> card >> door;

    std::unordered_set<Number> toFind;
    toFind.insert(card);
    toFind.insert(door);

    std::unordered_map<Number, Number> calculated;
    Number iteration = 0;
    Number value = 1;
    Number subject = 7;

    std::clog << "Running initial hack" << std::endl;

    while (!toFind.empty()) {
        calculated[value] = iteration++;
        toFind.erase(value);

        value *= subject;
        value %= 20201227;
    }

    std::clog << "Got " << calculated.size() << " values" << std::endl;

    std::clog << "card iterations: " << calculated[card] << std::endl;
    std::clog << "door iterations: " << calculated[door] << std::endl;

    auto cardLoops = calculated[card];
    auto doorLoops = calculated[door];

    value = 1;
    subject = door;
    for (Number i = 0; i < cardLoops; ++i) {
        value *= subject;
        value %= 20201227;
    }

    std::clog << "Key1: " << value << std::endl;

    value = 1;
    subject = card;
    for (Number i = 0; i < doorLoops; ++i) {
        value *= subject;
        value %= 20201227;
    }

    std::clog << "Key2: " << value << std::endl;

    return 0;
}