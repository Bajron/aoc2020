#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using Number = uint64_t;

struct Command {
    std::string type;
    Number a, b;
};

struct Execution {
    std::unordered_map<Number, Number> mem;
    Number andMask{~Number{0}};
    Number orMask{0};

    void run(const std::vector<Command> &commands) {
        for (const auto &c : commands) {
            if (c.type == "mask") {
                andMask = c.a;
                orMask = c.b;
            } else if (c.type == "mem") {
                mem[c.a] = ((c.b & andMask) | orMask);
            } else {
                std::clog << "unknown operation " << c.type << std::endl;
            }
        }
    }

    void runAlternative(const std::vector<Command> &commands) {
        Number floating;
        Number popCount;

        for (const auto &c : commands) {
            if (c.type == "mask") {
                andMask = c.a;
                orMask = c.b;

                auto affected = ((~andMask) | orMask);

                Number f = (Number{1} << 36) - 1;
                f &= ~affected;

                Number pc = 0, bit = 1;
                for (Number i = 0; i < 36; ++i, bit <<= 1) {
                    if ((bit & f) > 0)
                        ++pc;
                }
                floating = f;
                popCount = pc;

                std::clog << "Mask with " << popCount << " floating bits" << std::endl;
            } else if (c.type == "mem") {
                const auto addrBase = (c.a | orMask) & ~(floating);

                for (Number n = 0; n < (Number{1} << popCount); ++n) {
                    Number set = 0;
                    Number orMaskForAddr{0};
                    for (Number bit = 1; set < popCount; bit <<= Number{1}) {
                        if (bit & floating) {
                            const auto nMask = (Number{1} << set);
                            if (nMask & n) {
                                orMaskForAddr |= bit;
                            }
                            ++set;
                        }
                    }

                    mem[addrBase | orMaskForAddr] = c.b;
                }
            } else {
                std::clog << "unknown operation " << c.type << std::endl;
            }
        }
    }
};

int main(int argc, char args[]) {
    std::string line;

    std::vector<Command> input;

    do {
        std::getline(std::cin, line);
        if (!std::cin)
            break;

        std::replace(line.begin(), line.end(), '[', ' ');
        std::replace(line.begin(), line.end(), ']', ' ');
        std::replace(line.begin(), line.end(), '=', ' ');

        Command cmd;
        std::istringstream ss{line};
        ss >> cmd.type;

        if (cmd.type == "mask") {
            std::string tmp;
            ss >> tmp;

            Number andMask{~Number{0}}, orMask{0}, bit{1};
            for (auto it = tmp.rbegin(); it != tmp.rend();
                 ++it, bit <<= Number{1}) {
                if (*it == '0') {
                    andMask = andMask & (~bit);
                } else if (*it == '1') {
                    orMask = orMask | bit;
                }
            }
            cmd.a = andMask;
            cmd.b = orMask;
        } else if (cmd.type == "mem") {
            ss >> cmd.a >> cmd.b;
        } else {
            std::clog << "unexpected input" << std::endl;
            continue;
        }
        input.emplace_back(std::move(cmd));
    } while (std::cin);

    const auto run = [](const auto &input, const auto &mfn,
                        const std::string &info) {
        Execution ex;
        (ex.*mfn)(input);

        Number sum = 0;
        for (const auto [_, v] : ex.mem) {
            sum += v;
        }
        std::cout << info << ": sum is " << sum << std::endl;
    };

    run(input, &Execution::run, "value masking");
    run(input, &Execution::runAlternative, "address masking");

    return 0;
}