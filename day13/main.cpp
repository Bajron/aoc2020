#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using Number = int64_t;

int main(int argc, char args[]) {
    Number timestamp;
    std::cin >> timestamp;

    std::string line;
    std::getline(std::cin, line);

    std::vector<Number> buses;
    std::getline(std::cin, line);

    std::replace(line.begin(), line.end(), 'x', '0');
    std::replace(line.begin(), line.end(), ',', ' ');

    std::istringstream ss{line};
    do {
        Number n;
        ss >> n;
        if (ss)
            buses.emplace_back(n);
    } while (ss);

    std::clog << "Read " << buses.size() << " buses, my timestamp is "
              << timestamp << std::endl;

    auto waitMin = timestamp;
    auto choice = buses.front();

    for (auto b : buses) {
        if (b == 0)
            continue;
        const auto rest = timestamp % b;
        const auto wait = b - rest;

        if (wait < waitMin) {
            waitMin = wait;
            choice = b;
        }
    }

    std::cout << "Choose " << choice << " and wait " << waitMin << " answer is "
              << choice * waitMin << std::endl;

    std::clog << "Trying something..." << std::endl;
    Number t = 0;

    std::size_t incFits = 0;
    std::size_t incCandidate = 0;
    Number seenCandidate = 0;
    Number inc = buses[0];
    bool found = false;

    while (!found) {
        std::clog << "t is " << t << std::endl;
        std::size_t i = 1;
        while (i < buses.size() &&
               (buses[i] == 0 || ((t + i) % buses[i]) == 0)) {
            ++i;
        }
        if (i == buses.size()) {
            found = true;
            break;
        }
        --i;
        std::clog << "Matched " << i << " buses" << std::endl;
        if (i > incFits) {
            if (incCandidate == i) {
                incFits = i;
                inc = t - seenCandidate;
                std::clog << "incrementing by " << inc << " now" << std::endl;
            } else {
                incCandidate = i;
                seenCandidate = t;
            }
        }
        t += inc;
    }

    std::cout << "Umm... t = " << t << " ?" << std::endl;

    return 0;
}