#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using Ticket = std::vector<int>;

struct Rule {
    int lower;
    int upper;

    bool match(int value) const { return lower <= value && value <= upper; }
};

struct Note {
    Rule r1;
    Rule r2;

    bool match(int value) const { return r1.match(value) || r2.match(value); }
};

using Notes = std::unordered_map<std::string, Note>;

bool matchAnyNote(int value, const Notes &notes) {
    for (const auto &[name, note] : notes) {
        if (note.match(value))
            return true;
    }
    return false;
}

using Positions = std::unordered_map<std::string, std::vector<std::size_t>>;

void log(const Positions &positions) {
    for (const auto &[name, pos] : positions) {
        std::clog << name << " can be on ";
        for (auto p : pos) {
            std::clog << p << ", ";
        }
        std::clog << std::endl;
    }
}

void analyse(const Ticket &my, const std::vector<Ticket> &nearby,
             const Notes &notes) {
    std::vector<int> invalids;

    std::vector<Ticket> validTickets;

    for (const auto &t : nearby) {
        bool invalid = false;
        for (const auto field : t) {
            if (!matchAnyNote(field, notes)) {
                invalids.push_back(field);
                invalid = true;
            }
        }
        if (!invalid)
            validTickets.push_back(t);
    }

    const auto rate = std::accumulate(invalids.begin(), invalids.end(), 0);
    std::cout << "Invalids rate: " << rate << std::endl;

    Positions positions;

    for (const auto &[name, note] : notes) {
        for (std::size_t i = 0; i < my.size(); ++i) {
            bool matches = true;
            for (const auto &t : validTickets) {
                if (!note.match(t[i])) {
                    matches = false;
                    break;
                }
            }

            if (matches) {
                positions[name].push_back(i);
            }
        }
    }

    log(positions);
    std::clog << std::endl;

    std::unordered_map<std::string, std::size_t> definite;
    bool inferredSomething;

    do {
        inferredSomething = false;

        for (const auto &[name, pos] : positions) {
            if (pos.size() == 1) {
                if (definite.find(name) == definite.end()) {
                    const auto p = pos.front();
                    definite[name] = p;

                    inferredSomething = true;

                    for (auto &[_, pos2] : positions) {
                        const auto e = std::remove(pos2.begin(), pos2.end(), p);
                        pos2.erase(e, pos2.end());
                    }
                }
            }
        }
    } while (inferredSomething);

    log(positions);
    std::clog << "positions had " << positions.size() << " entries\n"
              << std::endl;

    for (const auto &[name, pos] : definite) {
        std::clog << name << " is on " << pos << std::endl;
    }
    std::clog << "definite have " << definite.size() << " entries\n"
              << std::endl;

    int64_t product = 1;
    for (const auto &[name, pos] : definite) {
        if (name.find("departure") == 0) {
            std::clog << "Taking " << name << "[" << pos << "] with value "
                      << my[pos] << std::endl;
            product *= my[pos];
        }
    }

    std::cout << "Final product is " << product << std::endl;
}

int main(int argc, char args[]) {
    Notes notes;

    std::string line;
    std::getline(std::cin, line);
    while (!line.empty()) {
        const auto sep = line.find(':');
        const auto name = line.substr(0, sep);
        line = line.substr(sep + 1);
        std::replace(line.begin(), line.end(), '-', ' ');
        std::replace(line.begin(), line.end(), 'o', ' ');
        std::replace(line.begin(), line.end(), 'r', ' ');

        Note note;
        std::istringstream ss{line};
        ss >> note.r1.lower >> note.r1.upper >> note.r2.lower >> note.r2.upper;

        notes[name] = std::move(note);

        std::getline(std::cin, line);
    }

    std::getline(std::cin, line);
    assert(line == "your ticket:");

    std::getline(std::cin, line);
    std::replace(line.begin(), line.end(), ',', ' ');

    Ticket myTicket;
    {
        std::istringstream ss{line};
        while (ss) {
            int n;
            ss >> n;
            if (ss)
                myTicket.push_back(n);
        }
    }

    std::getline(std::cin, line);
    assert(line.empty());

    std::getline(std::cin, line);
    assert(line == "nearby tickets:");

    std::vector<Ticket> otherTickets;
    while (std::cin) {
        std::getline(std::cin, line);
        if (!std::cin)
            break;

        std::replace(line.begin(), line.end(), ',', ' ');

        Ticket other;
        other.reserve(myTicket.size());

        std::istringstream ss{line};
        while (ss) {
            int n;
            ss >> n;
            if (ss)
                other.push_back(n);
        }
        otherTickets.emplace_back(std::move(other));
    }

    std::clog << "Read notes: " << notes.size() << std::endl;
    std::clog << "Read nearby tickets: " << otherTickets.size() << std::endl;

    analyse(myTicket, otherTickets, notes);

    return 0;
}