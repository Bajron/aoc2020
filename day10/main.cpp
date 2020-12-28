#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_set>
#include <vector>

using Number = std::int64_t;

Number clusterOfOnesPossibilities(Number size) {
    if (size < 0)
        return 0;
    if (size == 0)
        return 1;
    if (size <= 2)
        return size;
    if (size == 3)
        return 4;
    // if (size == 4)
    //     return 7;

    const auto f = &clusterOfOnesPossibilities;
    // Note: actual test data does not reach here :D
    if (size > 4)
        std::clog << "Actual formula not tested" << std::endl;

    // This results from
    // - fix edges to 1..1, let anything happen inside (size - 2)
    // - fix edges to 2..1 or 1..2, let anything happen inside (size - 3)
    // - fix edges to 2..2 or 3..1 or 1..3, anything happen inside (size - 4)
    // - and so on
    return f(size - 2) + 2 * f(size - 3) + 3 * f(size - 4) + 2 * f(size - 5) +
           f(size - 6);
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

    std::sort(numbers.begin(), numbers.end());

    numbers.insert(numbers.begin(), 0);
    numbers.insert(numbers.end(), numbers.back() + 3);

    std::array<Number, 4> counts{};
    {
        for (std::size_t i = 1; i < numbers.size(); ++i) {
            ++counts[numbers[i] - numbers[i - 1]];
        }
    }

    for (std::size_t i = 0; i < counts.size(); ++i) {
        std::clog << "count " << i << ": " << counts[i] << std::endl;
    }
    std::cout << "Answer is " << counts[1] * counts[3] << std::endl;

    // Now here we know our data has only clusters of 1 and 3, and we are going
    // to exploit that in the solution
    // FIXME: this is not general solution

    std::vector<Number> oneClusters;
    {
        Number currentDiff = numbers[1] - numbers[0];
        Number currentCount = 1;

        for (std::size_t i = 2; i < numbers.size(); ++i) {
            const auto diff = numbers[i] - numbers[i - 1];
            if (diff == currentDiff) {
                ++currentCount;
            } else {
                if (currentDiff == 1) {
                    oneClusters.push_back(currentCount);
                }
                currentDiff = diff;
                currentCount = 1;
            }
        }
    }

    Number r = 1;
    for (auto n : oneClusters) {
        std::clog << "Cluster of ones of size: " << n << " -> "
                  << clusterOfOnesPossibilities(n) << std::endl;
        r *= clusterOfOnesPossibilities(n);
    }

    std::cout << "Answer is " << r << std::endl;

    return 0;
}