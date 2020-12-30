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

using Number = std::int64_t;

Number score(const std::queue<Number> &deck) {
    auto d = deck;
    Number score = 0;
    Number multiplier = d.size();
    while (!d.empty()) {
        score += multiplier * d.front();
        d.pop();
        --multiplier;
    }
    return score;
}

struct CombatGame {
    std::queue<Number> deck1;
    std::queue<Number> deck2;

    void step() {
        auto one = deck1.front();
        deck1.pop();
        auto two = deck2.front();
        deck2.pop();

        std::clog << "P1: " << one << " P2: " << two << std::endl;

        if (one > two) {
            deck1.push(one);
            deck1.push(two);
        } else if (two > one) {
            deck2.push(two);
            deck2.push(one);
        } else {
            std::clog << "Wait a minute..." << std::endl;
        }
    }

    bool isDone() const { return deck1.empty() || deck2.empty(); }

    void run() {
        while (!isDone()) {
            step();
        }
    }
};

using Deck = std::queue<Number>;

struct RecursiveCombatGame {
    static std::map<std::pair<Deck, Deck>, int> cache;

    std::queue<Number> deck1;
    std::queue<Number> deck2;

    std::optional<int> winner;

    std::set<std::pair<Deck, Deck>> configurations;

    void step() {
        {
            auto configuration = std::make_pair(deck1, deck2);

            if (configurations.find(configuration) != configurations.end()) {
                std::clog << "Emergency stop agains recursion!" << std::endl;
                winner = 1;
                return;
            }

            configurations.insert(std::move(configuration));
        }

        int localWinner = 0;

        auto one = deck1.front();
        deck1.pop();
        auto two = deck2.front();
        deck2.pop();

        // std::clog << "P1: " << one << " P2: " << two << std::endl;

        if (Number(deck1.size()) >= one && Number(deck2.size()) >= two) {
            std::clog << "Subgame!" << std::endl;

            auto copy1 = deck1, copy2 = deck2;
            Deck d1, d2;
            while (d1.size() != one) {
                d1.push(copy1.front());
                copy1.pop();
            }
            while (d2.size() != two) {
                d2.push(copy2.front());
                copy2.pop();
            }

            RecursiveCombatGame subGame{std::move(d1), std::move(d2)};
            subGame.run();
            localWinner = *subGame.winner;
        } else {
            if (one > two) {
                localWinner = 1;
            } else if (two > one) {
                localWinner = 2;
            } else {
                std::clog << "Wait a minute..." << std::endl;
            }
        }

        if (localWinner == 1) {
            deck1.push(one);
            deck1.push(two);
        } else if (localWinner == 2) {
            deck2.push(two);
            deck2.push(one);
        } else {
            std::clog << "How?..." << std::endl;
        }

        if (deck1.empty())
            winner = 2;
        if (deck2.empty())
            winner = 1;
    }

    bool isDone() const { return static_cast<bool>(winner); }

    void run() {
        auto configuration = std::make_pair(deck1, deck2);
        auto it = cache.find(configuration);
        if (it != cache.end()) {
            std::clog << "This game was played already" << std::endl;
            winner = it->second;
            return;
        }

        while (!isDone()) {
            step();
        }
        cache[configuration] = *winner;
    }
};

std::map<std::pair<Deck, Deck>, int> RecursiveCombatGame::cache;

int main(int argc, char args[]) {
    std::string line;
    std::queue<Number> input1, input2;
    do {
        std::getline(std::cin, line);
        assert(line == "Player 1:");
        while (true) {
            std::getline(std::cin, line);
            if (line.empty() || !std::cin)
                break;
            input1.push(std::stoll(line));
        }

        std::getline(std::cin, line);
        assert(line == "Player 2:");

        while (true) {
            std::getline(std::cin, line);
            if (line.empty() || !std::cin)
                break;
            input2.push(std::stoll(line));
        }
    } while (std::cin);

    std::clog << "Deck 1: " << input1.size() << " Deck 2: " << input2.size() << std::endl;

    CombatGame game{input1, input2};
    game.run();

    std::cout << "P1 score: " << score(game.deck1) << std::endl;
    std::cout << "P2 score: " << score(game.deck2) << std::endl;

    RecursiveCombatGame rcg{input1, input2};
    rcg.run();

    std::cout << "Winner is " << *rcg.winner << std::endl;
    std::cout << "P1 score: " << score(rcg.deck1) << std::endl;
    std::cout << "P2 score: " << score(rcg.deck2) << std::endl;

    return 0;
}