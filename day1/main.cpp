#include <iostream>
#include <vector>
#include <unordered_set>

std::pair<long,long> summingTo(const std::vector<long>& input, const long target) {
    std::unordered_set<long> hash{input.begin(), input.end()};
    for (auto number : input) {
        if (hash.find(target - number) != hash.end()) {
            return {number, target};
        }
    }
    return {-1, -1};
}

int main(int argc, char args[]) {
    std::vector<long> input;
    long number;
    do {
        std::cin >> number;
        if (std::cin.good()) {
            input.push_back(number);
        }
    } while(std::cin.good());

    auto p = summingTo(input, 2020);

    std::clog << "Found: " << p.first << " + " << p.second << std::endl;
    std::cout << p.first * p.second << std::endl;

    return 0;
}