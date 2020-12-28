#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Passport = std::unordered_map<std::string, std::string>;

// "cid" is optional
const std::set<std::string> required = {
    "byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid",
};

const std::set<std::string> validEyeColor = {
    "amb", "blu", "brn", "gry", "grn", "hzl", "oth",
};

bool isValid(const Passport &p) {
    for (const auto &key : required) {
        if (p.find(key) == p.end())
            return false;
    }
    return true;
}

bool between(int v, int lower, int upper) { return lower <= v && v <= upper; }

bool fourDigitsBetween(const std::string &s, int lower, int upper) {
    if (s.size() != 4)
        return false;
    if (!std::all_of(s.begin(), s.end(), std::isdigit))
        return false;
    if (!between(std::stoi(s), lower, upper))
        return false;

    return true;
}

// byr (Birth Year) - four digits; at least 1920 and at most 2002.
// iyr (Issue Year) - four digits; at least 2010 and at most 2020.
// eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
// hgt (Height) - a number followed by either cm or in:
// If cm, the number must be at least 150 and at most 193.
// If in, the number must be at least 59 and at most 76.
// hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
// ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
// pid (Passport ID) - a nine-digit number, including leading zeroes.
// cid (Country ID) - ignored, missing or not.
bool isValidStrict(const Passport &p) {
    if (!isValid(p))
        return false;

    if (!fourDigitsBetween(p.at("byr"), 1920, 2002))
        return false;
    if (!fourDigitsBetween(p.at("iyr"), 2010, 2020))
        return false;
    if (!fourDigitsBetween(p.at("eyr"), 2020, 2030))
        return false;

    const auto &h = p.at("hgt");
    if (h.size() < 3)
        return false;
    const auto hUnit = h.substr(h.size() - 2);
    const auto hValue = std::stoi(h.substr(0, h.size() - 2));
    if (hUnit == "cm") {
        if (!between(hValue, 150, 193))
            return false;
    } else if (hUnit == "in") {
        if (!between(hValue, 59, 76))
            return false;
    } else {
        return false;
    }

    const auto &hcl = p.at("hcl");
    if (hcl.size() != 7 || hcl[0] != '#')
        return false;
    if (!std::all_of(hcl.begin() + 1, hcl.end(), [](const auto ch) {
            return ('a' <= ch && ch <= 'f') || ('0' <= ch && ch <= '9');
        }))
        return false;

    const auto &ecl = p.at("ecl");
    if (validEyeColor.find(ecl) == validEyeColor.end())
        return false;

    const auto &pid = p.at("pid");
    if (pid.size() != 9) return false;
    if (!std::all_of(pid.begin(), pid.end(), std::isdigit))
        return false;

    return true;
}

int countValid(const std::vector<Passport> &passports,
               bool (*isValid)(const Passport &)) {
    int counter = 0;

    for (const auto &p : passports) {
        if (isValid(p))
            ++counter;
    }

    return counter;
}

int main(int argc, char args[]) {
    std::vector<Passport> input;

    std::string buffer, line;

    do {
        buffer.clear();
        line.clear();
        do {
            std::getline(std::cin, line);
            if (std::cin && !line.empty()) {
                buffer += ' ';
                buffer += line;
            }
        } while (std::cin && !line.empty());

        Passport passport;
        std::istringstream ss{buffer};
        do {
            ss >> buffer;
            if (!ss)
                break;

            const auto key = buffer.substr(0, 3);
            buffer.erase(buffer.begin(), buffer.begin() + 4);

            passport[key] = buffer;
        } while (ss);

        if (!passport.empty())
            input.emplace_back(std::move(passport));

    } while (std::cin);

    for (const auto &p : input) {
        for (const auto &[k, v] : p) {
            std::clog << k << ":" << v << " ";
        }
        std::clog << std::endl;
    }

    std::cout << "Valid passports: " << countValid(input, &isValid)
              << std::endl;
    std::cout << "Valid passports strict: " << countValid(input, &isValidStrict)
              << std::endl;

    return 0;
}