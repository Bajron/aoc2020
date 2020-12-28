#include <iostream>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

std::pair<long, long> summingTo(const std::vector<long> &input,
                                const long target) {
    std::unordered_set<long> hash{input.begin(), input.end()};
    for (auto number : input) {
        const auto matching = target - number;
        if (hash.find(matching) != hash.end()) {
            return {number, matching};
        }
    }
    return {0, 0};
}

std::tuple<long, long, long> threeSummingTo(const std::vector<long> &input,
                                            const long target) {
    std::unordered_set<long> hash{input.begin(), input.end()};
    for (std::size_t i = 0; i < input.size(); ++i) {
        for (std::size_t j = i; j < input.size(); ++j) {
            const auto matching = target - (input[i] + input[j]);
            if (hash.find(matching) != hash.end()) {
                return {input[i], input[j], matching};
            }
        }
    }
    return {0, 0, 0};
}

int main(int argc, char args[]) {
    std::vector<long> input;
    long number;
    do {
        std::cin >> number;
        if (std::cin.good()) {
            input.push_back(number);
        }
    } while (std::cin.good());

    auto p = summingTo(input, 2020);

    std::clog << "Found: " << p.first << " + " << p.second << std::endl;
    std::cout << p.first * p.second << std::endl;

    auto t = threeSummingTo(input, 2020);
    std::clog << "Found: " << std::get<0>(t) << " + " << std::get<1>(t) << " + "
              << std::get<2>(t) << std::endl;
    std::cout << std::get<0>(t) * std::get<1>(t) * std::get<2>(t) << std::endl;

    return 0;
}