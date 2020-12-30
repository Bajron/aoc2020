#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void bigGame(const std::string &init) {
    std::list<int> state;

    for (auto ch : init) {
        state.push_back(static_cast<int>(ch - '0'));
    }

    const int min = *std::min_element(state.begin(), state.end());
    int maxSoFar = *std::max_element(state.begin(), state.end());
    while (state.size() != 1000000) {
        ++maxSoFar;
        state.push_back(maxSoFar);
    }
    const int max = maxSoFar;

    std::clog << "initialized big game " << min << " - " << max << std::endl;

    // Hack yeah!
    std::unordered_map<int, std::list<int>::iterator> quickFind;
    for (auto it = state.begin(); it != state.end(); ++it) {
        quickFind[*it] = it;
    }

    std::clog << "prepared search cache" << std::endl;

    std::vector<int> out;
    auto cur = state.begin();

    for (int i = 0; i < 10000000; ++i) {
        if (i % 1000000 == 0) {
            std::clog << i / 1000000 << " done..." << std::endl;
        }
        if (state.size() < 20) {
            for (const auto n : state) {
                std::clog << n << " ";
            }
            std::clog << std::endl;
        }
        auto copyOutIt = cur;
        ++copyOutIt;
        for (int j = 0; j < 3; ++j) {
            if (copyOutIt == state.end()) {
                copyOutIt = state.begin();
            }
            out.push_back(*copyOutIt);
            copyOutIt = state.erase(copyOutIt);
        }

        int toFind = *cur - 1;
        if (toFind < min)
            toFind = max;
        while (std::find(out.begin(), out.end(), toFind) != out.end()) {
            --toFind;
            if (toFind < min)
                toFind = max;
        }

        auto toInsert = quickFind[toFind];
        ++toInsert;

        auto inserted = state.insert(toInsert, out.begin(), out.end());
        out.clear();

        // update search cache
        for (int j = 0; j < 3; ++j) {
            quickFind[*inserted] = inserted;
            ++inserted;
        }

        ++cur;
        if (cur == state.end())
            cur = state.begin();
    }

    auto itOne = std::find(state.begin(), state.end(), 1);
    std::rotate(state.begin(), itOne, state.end());

    auto it = state.begin();
    ++it;
    int64_t a = *it++;
    int64_t b = *it++;
    
    std::clog << "Magic nubmers are " << a << " and " << b << std::endl;
    std::cout << "The answer: " << a*b << std::endl;
}

int main(int argc, char args[]) {
    //std::string initialState = "389125467";
    std::string initialState = "389547612";

    std::string state = initialState;
    std::size_t n = state.size();

    std::string sorted = state;
    std::sort(sorted.begin(), sorted.end());

    std::clog << "initial: " << state << std::endl;
    std::clog << "sorted:  " << sorted << std::endl;

    std::string out = "XXX";
    std::size_t curPos = 0;

    for (int i = 0; i < 100; ++i) {
        std::clog << i << ": " << state << std::endl;

        char cur = state[curPos];

        for (int j = 0; j < 3; ++j) {
            const std::size_t p = (curPos + 1 + j) % n;
            std::swap(out[j], state[p]);
        }

        auto newEnd = std::remove(state.begin(), state.end(), 'X');
        state.resize(n - 3);

        auto rank = sorted.find(cur);

        auto dstRank = (rank - 1 + n) % n;
        auto dstChar = sorted[dstRank];
        auto dstLoc = state.find(dstChar);
        while (dstLoc == std::string::npos) {
            --dstRank;
            dstRank += n;
            dstRank %= n;

            dstChar = sorted[dstRank];
            dstLoc = state.find(dstChar);
        }

        state.insert(state.begin() + (dstLoc + 1) % n, out.begin(), out.end());
        out.clear();
        out.resize(3, 'X');

        curPos = state.find(cur);
        ++curPos;
        curPos %= n;
    };

    std::rotate(state.begin(), state.begin() + state.find('1'), state.end());

    std::cout << "Normalized state: " << state << std::endl;
    std::cout << "Answer is " << state.substr(1) << std::endl;

    bigGame(initialState);

    return 0;
}