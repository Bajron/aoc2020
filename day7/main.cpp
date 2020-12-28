#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using Label = std::string;
using Count = int;

struct Rule {
    Label label;
    std::unordered_map<Label, Count> contains;
};
using WeightedLabelRelation =
    std::unordered_map<Label, std::unordered_map<Label, Count>>;

WeightedLabelRelation createContainsWeighted(const std::vector<Rule> &rules) {
    WeightedLabelRelation containsWeighted;

    for (const auto &r : rules) {
        containsWeighted[r.label] = r.contains;
    }

    return containsWeighted;
}

Count totalBagCount(const WeightedLabelRelation &relation, const Label &label) {
    Count total = 0;
    for (const auto &[ch, count] : relation.at(label)) {
        // TODO: might use cache
        total += count * (1 + totalBagCount(relation, ch));
    }
    return total;
}

using LabelRelation = std::unordered_map<Label, std::unordered_set<Label>>;

LabelRelation createCanBeContainedBy(const std::vector<Rule> &rules) {
    LabelRelation canBeContainedBy;

    for (const auto &r : rules) {
        canBeContainedBy[r.label];
        for (const auto &[label, _] : r.contains) {
            canBeContainedBy[label].insert(r.label);
        }
    }

    return canBeContainedBy;
}

void visitSubtree(const LabelRelation &relation, const Label &label,
                  std::unordered_set<Label> &visited) {
    const auto [_, added] = visited.insert(label);
    if (!added)
        return;
    for (const auto &ch : relation.at(label)) {
        visitSubtree(relation, ch, visited);
    }
}

int main(int argc, char args[]) {
    std::vector<Rule> rules;
    std::string specifier, color, skip, number;

    do {
        std::cin >> specifier;
        if (!std::cin)
            break;
        std::cin >> color;

        Rule rule;
        rule.label = specifier + ' ' + color;
        std::cin >> skip >> skip; // "bags contain"
        std::cin >> number;
        if (number == "no") {
            std::cin >> skip >> skip; // "other bags."
        } else {
            do {
                std::cin >> specifier >> color;
                rule.contains[specifier + ' ' + color] = std::stoi(number);
                std::cin >> skip; // "bag," "bags,"

                if (skip.back() == ',')
                    std::cin >> number;
            } while (skip.back() == ',');
        }
        rules.emplace_back(std::move(rule));
    } while (std::cin);

    std::clog << "Read " << rules.size() << " rules" << std::endl;

    const auto canBeContainedBy = createCanBeContainedBy(rules);
    std::clog << "Got " << canBeContainedBy.size() << " labels" << std::endl;

    std::unordered_set<Label> visited;
    visitSubtree(canBeContainedBy, "shiny gold", visited);

    std::clog << visited.size() << " nodes in shiny gold subtree" << std::endl;
    std::cout << "The answer is " << visited.size() - 1 << std::endl;

    const auto containsWeighted = createContainsWeighted(rules);
    std::clog << "Got " << containsWeighted.size() << " labels" << std::endl;

    std::cout << "Total bags in shiny gold: "
              << totalBagCount(containsWeighted, "shiny gold") << std::endl;

    return 0;
}