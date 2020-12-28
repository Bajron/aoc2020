#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Pass = std::string;

std::vector<unsigned long> toBinary(const std::vector<Pass> &input) {
    std::vector<unsigned long> ret;
    ret.reserve(input.size());

    for (const auto &s : input) {
        auto buffer = s;
        std::replace(buffer.begin(), buffer.end(), 'F', '0');
        std::replace(buffer.begin(), buffer.end(), 'B', '1');
        std::replace(buffer.begin(), buffer.end(), 'R', '1');
        std::replace(buffer.begin(), buffer.end(), 'L', '0');

        ret.push_back(std::stoul(buffer, 0, 2));
    }
    return ret;
}

int main(int argc, char args[]) {
    std::vector<Pass> input;
    std::string line;

    do {
        std::getline(std::cin, line);
        if (std::cin)
            input.push_back(line);
    } while (std::cin);

    std::clog << "Read " << input.size() << " elements" << std::endl;

    auto processed = toBinary(input);

    for (std::size_t i = 0; i < input.size(); ++i) {
        std::clog << input[i] << " -> " << processed[i] << " "
                  << processed[i] / 8 << " " << processed[i] % 8 << std::endl;
    }

    std::cout << "Max element: "
              << *std::max_element(processed.begin(), processed.end())
              << std::endl;

    std::sort(processed.begin(), processed.end());

    for (std::size_t i = 0; i < input.size() - 1; ++i) {
        if (processed[i] + 2 == processed[i + 1]) {
            std::cout << "I can sit on " << processed[i] + 1 << std::endl;
        }
    }
    return 0;
}