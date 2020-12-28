#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct Operation {
    std::string code;
    int argument;
};

using Program = std::vector<Operation>;

struct Execution {
    const Program program;
    int accumulator{0};
    int instruction{0};

    void singleOp() {
        const auto &op = program[instruction];
        if (op.code == "acc") {
            accumulator += op.argument;
            ++instruction;
        } else if (op.code == "jmp") {
            instruction += op.argument;
        } else if (op.code == "nop") {
            ++instruction;
        } else {
            std::clog << "invalid operation " << op.code << std::endl;
            std::exit(1);
        }
    }

    void reportState() const {
        std::cout << "State acc: " << accumulator << " ip: " << instruction
                  << std::endl;
    }

    bool runWithLoopDetection() {
        const int size = static_cast<int>(program.size());
        std::vector<bool> visited(program.size(), false);
        while (instruction < size) {
            if (visited[instruction]) {
                return false;
            }

            visited[instruction] = true;
            singleOp();
        }

        return true;
    }
};

int main(int argc, char args[]) {
    Program program;
    do {
        Operation op;
        std::cin >> op.code >> op.argument;
        if (std::cin)
            program.emplace_back(std::move(op));
    } while (std::cin);

    Execution ex{program};
    if (!ex.runWithLoopDetection()) {
        std::cout << "Repeated execution!" << std::endl;
        ex.reportState();
    }

    std::vector<std::size_t> changeCandidates;
    for (std::size_t i = 0; i < program.size(); ++i) {
        if (program[i].code != "acc") {
            changeCandidates.push_back(i);
        }
    }

    std::unordered_map<std::string, std::string> map{{"jmp", "nop"},
                                                     {"nop", "jmp"}};
    for (const auto change : changeCandidates) {
        auto original = program[change].code;
        program[change].code = map[original];

        Execution ex{program};
        if (ex.runWithLoopDetection()) {
            std::cout << "Successful execution after changing " << change
                      << std::endl;
            ex.reportState();
        }
        std::swap(original, program[change].code);
    }

    return 0;
}