#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using Group = std::vector<std::string>;
using Input = std::vector<Group>;

long long countAnyYes(const Group &g) {
    std::unordered_set<char> counters;

    for (const auto &answers : g) {
        for (auto yes : answers) {
            counters.insert(yes);
        }
    }

    return static_cast<long long>(counters.size());
}

long long countAllYes(const Group &g) {
    std::unordered_map<char, std::size_t> counters;

    for (const auto &answers : g) {
        for (auto yes : answers) {
            ++counters[yes];
        }
    }

    return std::count_if(counters.begin(), counters.end(),
                         [&](const auto &p) { return p.second == g.size(); });
}

int main(int argc, char args[]) {
    Input input;
    std::string line;

    do {
        Group group;
        do {
            std::getline(std::cin, line);
            if (!line.empty() && std::cin) {
                group.push_back(line);
            }
        } while (!line.empty() && std::cin);

        if (!group.empty())
            input.emplace_back(std::move(group));
    } while (std::cin);

    std::clog << "Read " << input.size() << " groups" << std::endl;

    const auto run = [&input](long long (*policy)(const Group &),
                              const std::string &info) {
        std::vector<long long> yesCounts;
        yesCounts.reserve(input.size());

        for (const auto &g : input) {
            yesCounts.push_back(policy(g));
        }
        std::cout << "Sum of " << info << " for groups: "
                  << std::accumulate(yesCounts.begin(), yesCounts.end(), 0LL)
                  << std::endl;
    };

    run(countAnyYes, "any yes");
    run(countAllYes, "all yes");

    return 0;
}