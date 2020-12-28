#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

using Number = std::int64_t;

Number validate(const std::vector<Number> &numbers, const int window) {
    const int n = static_cast<int>(numbers.size());
    if (n < window) {
        std::clog << "Window is too large for the data " << n << " < " << window
                  << std::endl;
        return -2;
    }
    std::unordered_set<Number> windowHash{numbers.begin(),
                                          numbers.begin() + window};

    for (int i = window; i < n; ++i) {
        const auto checked = numbers[i];

        bool found = false;
        for (int j = i - window; j < i; ++j) {
            const auto need = checked - numbers[j];
            if (need != checked && windowHash.find(need) != windowHash.end()) {
                found = true;
                break;
            }
        }
        if (!found)
            return checked;

        windowHash.erase(numbers[i - window]);
        windowHash.insert(checked);
    }

    std::clog << "Sequence seems to be ok for window " << window << std::endl;
    return -1;
}

auto findRangeAddingTo(const std::vector<Number> &numbers,
                       const Number requested) {
    const int n = static_cast<int>(numbers.size());
    for (int start = 0; start < n; ++start) {
        Number sum = 0;
        int current = start;
        while (current < n && sum < requested) {
            sum += numbers[current];
            ++current;
        }
        if (sum == requested)
            return std::make_pair(numbers.begin() + start,
                                  numbers.begin() + current);
    }
    return std::make_pair(numbers.end(), numbers.end());
}

int main(int argc, char args[]) {
    std::vector<Number> numbers;
    Number tmp;
    do {
        std::cin >> tmp;
        if (std::cin)
            numbers.push_back(tmp);
    } while (std::cin);

    std::clog << "Read " << numbers.size() << " numbers" << std::endl;

    const auto run = [&numbers = std::as_const(numbers)](const int window) {
        auto v = validate(numbers, window);
        std::cout << "Answer for window " << window << ": " << v << std::endl;

        if (v < 0)
            return;

        auto sumRange = findRangeAddingTo(numbers, v);

        if (sumRange.first == sumRange.second) {
            std::clog << "Received empty range" << std::endl;
            return;
        }

        const auto mm = std::minmax_element(sumRange.first, sumRange.second);
        std::clog << "Min in range: " << *mm.first << std::endl;
        std::clog << "Max in range: " << *mm.second << std::endl;

        std::cout << "Weakness is: " << *mm.first + *mm.second << std::endl;
    };

    run(5);
    run(25);

    return 0;
}