#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Rule {
    char ch = '\0';
    std::vector<std::vector<int>> alternatives;
};

using Rules = std::unordered_map<std::size_t, Rule>;

std::vector<std::size_t> match(const std::string &s, std::size_t i,
                               std::size_t rule, const Rules &rules) {
    if (i >= s.size()) {
        return {};
    }
    const auto r = rules.at(rule);
    if (r.ch != '\0') {
        if (r.ch == s[i])
            return {1};
    }

    std::vector<std::size_t> ret;

    for (const auto &alt : r.alternatives) {
        std::vector<std::size_t> states{{0}};
        for (auto innerRule : alt) {
            std::vector<std::size_t> next;
            for (auto si : states) {
                auto matches = match(s, i + si, innerRule, rules);
                for (auto matchedLetters : matches) {
                    next.push_back(si + matchedLetters);
                }
            }
            states = next;
        }
        ret.insert(ret.end(), states.begin(), states.end());
    }
    return ret;
}

bool match(const std::string &s, const Rules &rules) {
    auto matchLengths = match(s, 0, 0, rules);
    return std::find(matchLengths.begin(), matchLengths.end(), s.size()) != matchLengths.end();
}

int main(int argc, char args[]) {
    Rules rules;

    std::string line;
    do {
        std::getline(std::cin, line);
        if (line.empty())
            break;

        std::size_t offset = line.find(':');
        int ruleNumber = std::stoi(line.substr(0, offset));
        Rule rule;

        std::size_t it = line.find('"');
        if (it != std::string::npos) {
            rule.ch = line[it + 1];
        } else {
            ++offset;

            do {
                it = line.find('|', offset);

                std::istringstream ss{line.substr(offset, it)};
                std::vector<int> ref;
                int r;
                while (ss) {
                    ss >> r;
                    if (ss)
                        ref.push_back(r);
                };
                rule.alternatives.emplace_back(std::move(ref));

                offset = it + 1;
            } while (it != std::string::npos);
        }
        rules[ruleNumber] = std::move(rule);
    } while (std::cin);

    std::vector<std::string> input;
    do {
        std::getline(std::cin, line);
        if (!std::cin)
            break;
        input.push_back(std::move(line));
    } while (std::cin);

    for (const auto &r : rules) {
        std::clog << "Rule " << r.first << " with "
                  << r.second.alternatives.size() << " alternatives"
                  << std::endl;
    }

    std::clog << input.size() << " messages to verify" << std::endl;

    const auto runMatching = [&]() {
        int counter = 0;
        for (const auto &m : input) {
            if (match(m, rules)) {
                std::clog << m << " matched" << std::endl;
                ++counter;
            }
        }

        std::cout << "Messages matched: " << counter << std::endl;
    };

    runMatching();

    std::clog << "Changing rules" << std::endl;

    rules[8] = Rule{'\0', {{42}, {42, 8}}};
    rules[11] = Rule{'\0', {{42, 31}, {42, 11, 31}}};

    runMatching();

    return 0;
}