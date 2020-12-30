#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Product {
    std::set<std::string> ingredients;
    std::vector<std::string> known;
};

struct Mapping {
    std::string known;
    std::string unknown;
};

int main(int argc, char args[]) {
    std::string line;
    std::string word;

    std::vector<Product> input;
    do {
        std::getline(std::cin, line);
        if (!std::cin)
            break;
        auto paren = line.find('(');

        std::set<std::string> ingredients;
        {
            std::istringstream ss{line.substr(0, paren)};
            while (ss) {
                ss >> word;
                if (ss)
                    ingredients.insert(word);
            }
        }

        std::vector<std::string> known;
        {
            auto knownPart = line.substr(line.find(' ', paren + 1));
            knownPart.pop_back(); // last paren
            std::size_t offset = 0;
            std::size_t end;
            do {
                end = knownPart.find(',', offset);
                while (knownPart[offset] == ' ')
                    ++offset;
                known.push_back(knownPart.substr(offset, end - offset));
                offset = end + 1;
            } while (end != std::string::npos);
        }
        input.push_back(Product{std::move(ingredients), std::move(known)});
    } while (std::cin);

    std::clog << "Read " << input.size() << " products" << std::endl;

    for (const auto &p : input) {
        std::clog << "got: " << p.ingredients.size() << " unknown and " << p.known.size() << " known" << std::endl;
    }

    if (input.size() < 10) {
        for (const auto &p : input) {
            std::copy(p.ingredients.begin(), p.ingredients.end(), std::ostream_iterator<std::string>(std::clog, " "));
            std::clog << " -- ";
            std::copy(p.known.begin(), p.known.end(), std::ostream_iterator<std::string>(std::clog, " "));
            std::clog << std::endl;
        }
    }

    std::unordered_map<std::string, std::set<std::string>> possibleWord;
    std::set<std::string> allUnknown;
    std::unordered_map<std::string, int> allUnknownAppearences;

    for (const auto &p : input) {
        for (const auto &w : p.ingredients) {
            allUnknown.insert(w);
            allUnknownAppearences[w]++;
        }
        for (const auto &k : p.known) {
            auto &possible = possibleWord[k];
            if (possible.empty()) {
                possible = p.ingredients;
            } else {
                std::set<std::string> intersection;
                std::set_intersection(possible.begin(), possible.end(), p.ingredients.begin(), p.ingredients.end(),
                                      std::inserter(intersection, intersection.end()));
                possible = intersection;
            }
        }
    }

    auto excluded = allUnknown;
    for (const auto &[word, pSet] : possibleWord) {
        for (const auto &unknown : pSet)
            excluded.erase(unknown);
    }

    int counter = 0;
    for (const auto &e : excluded) {
        std::clog << e << ", ";
        counter += allUnknownAppearences[e];
    }
    std::clog << std::endl;

    std::cout << "Answer is " << counter << std::endl;

    for (const auto &[word, pSet] : possibleWord) {
        std::clog << "word: " << pSet.size() << " options" << std::endl;
        std::copy(pSet.begin(), pSet.end(), std::ostream_iterator<std::string>(std::clog, ", "));
        std::clog << std::endl;
    }

    std::vector<Mapping> dictionary;

    bool inferredSomething;
    do {
        inferredSomething = false;
        for (const auto &[word, pSet] : possibleWord) {
            if (pSet.size() == 1) {
                std::string translation = *pSet.begin();
                dictionary.push_back(Mapping{word, translation});
                inferredSomething = true;

                for (auto &[_, ps] : possibleWord) {
                    ps.erase(translation);
                }
            }
        }
    } while (inferredSomething);

    for (const auto &[word, pSet] : possibleWord) {
        std::clog << "word: " << pSet.size() << " options" << std::endl;
    }

    std::sort(dictionary.begin(), dictionary.end(),
              [](const auto &lhs, const auto &rhs) { return lhs.known < rhs.known; });

    std::ostringstream oss;
    for (const auto &m : dictionary) {
        oss << m.unknown << ',';
    }
    auto ans = oss.str();
    ans.pop_back();

    std::cout << "The answer is " << ans << std::endl;

    return 0;
}