#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Policy {
    int lower;
    int upper;
    char letter;
};

bool isValid(const Policy &policy, const std::string &password) {
    std::unordered_map<char, int> counters;
    for (auto ch : password)
        ++counters[ch];
    const auto checkedCount = counters[policy.letter];
    return policy.lower <= checkedCount && checkedCount <= policy.upper;
}

bool isValidAlternative(const Policy &policy, const std::string &password) {
    const auto i = policy.lower - 1;
    const auto j = policy.upper - 1;
    const auto ch = policy.letter;
    return (password[i] == ch) != (password[j] == ch);
}

auto countValid(const std::vector<std::pair<Policy, std::string>> &input) {
    return std::count_if(input.begin(), input.end(), [](const auto &i) {
        return isValid(i.first, i.second);
    });
}
auto countValidAlternative(
    const std::vector<std::pair<Policy, std::string>> &input) {
    return std::count_if(input.begin(), input.end(), [](const auto &i) {
        return isValidAlternative(i.first, i.second);
    });
}

int main(int argc, char args[]) {
    Policy buffer;
    std::string password;
    char skip;

    std::vector<std::pair<Policy, std::string>> input;

    do {
        std::cin >> buffer.lower >> skip >> buffer.upper >> buffer.letter >>
            skip;
        std::cin >> password;
        if (!std::cin)
            break;
        std::clog << "Read " << buffer.lower << "-" << buffer.upper << " "
                  << buffer.letter << ": " << password << std::endl;
        input.emplace_back(buffer, password);
    } while (std::cin);

    std::cout << "Valid passwords: " << countValid(input) << std::endl;
    std::cout << "Valid passwords alternative: " << countValidAlternative(input)
              << std::endl;

    return 0;
}