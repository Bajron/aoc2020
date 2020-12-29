#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using Number = uint64_t;
using Round = Number;

struct Game {
    Round round{1};
    std::unordered_map<Number, Round> occurences;

    Number last;
    Number lastAge;
    bool lastNew;

    void put(Number n) {
        const auto it = occurences.find(n);
        if (it == occurences.end()) {
            lastNew = true;
            occurences.insert(std::make_pair(n, round));
        } else {
            lastNew = false;
            lastAge = round - it->second;
            it->second = round;
        }

        last = n;
    }

    void run(std::vector<Number> initial, const Round rounds) {
        for (const auto n : initial) {
            put(n);
            ++round;
        }

        while (round <= rounds) {
            if (lastNew) {
                put(0);
            } else {
                put(lastAge);
            }
            ++round;
        };
    }
};

int main(int argc, char args[]) {
    std::vector<Number> sample1{{0, 3, 6}};
    std::vector<Number> sample2{{3, 1, 2}};
    std::vector<Number> input{{1, 2, 16, 19, 18, 0}};

    const auto run = [](const auto &input, const std::string &info) {
        Game gs;
        gs.run(input, 2020);
        std::clog << info << ": " << gs.last << std::endl;
    };

    run(sample1, "sample1");
    run(sample2, "sample2");
    run(input, "input");

const auto runMore = [](const auto &input, const std::string &info) {
        Game gs;
        gs.run(input, 30000000);
        std::clog << info << ": " << gs.last << std::endl;
    };

    runMore(sample1, "sample1");
    runMore(sample2, "sample2");
    runMore(input, "input");

    return 0;
}