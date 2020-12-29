#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Number = std::int64_t;

struct Token {
    char type;
    Number asNumber{-1};
};

using Tokenized = std::vector<Token>;

Number add(Number a, Number b) { return a + b; }
Number mul(Number a, Number b) { return a * b; }

Number process(const Tokenized &tokens, std::size_t &i) {
    std::vector<Number> stack;
    decltype(&add) function = nullptr;

    while (i < tokens.size()) {
        const auto &t = tokens[i];
        // std::clog << "processing: " << t.type << std::endl;
        switch (t.type) {
        case '(':
            ++i;
            stack.push_back(process(tokens, i));
            assert(tokens[i].type == ')');
            break;
        case ')':
            assert(stack.size() == 1);
            return stack.back();
        case '+':
            function = &add;
            break;
        case '*':
            function = &mul;
            break;
        case '0':
            stack.push_back(t.asNumber);
            break;
        default:
            std::clog << "Oh dear..." << std::endl;
        }

        ++i;

        if (stack.size() > 1) {
            Number a = stack.back();
            stack.pop_back();
            Number b = stack.back();
            stack.pop_back();
            stack.push_back(function(a, b));
        }
    }

    return stack.back();
}

struct Node {
    explicit Node(char t, Number v = -1) : type{t}, value{v} {}

    char type;
    Number value;
    Node *left = nullptr;
    Node *right = nullptr;
};

Node *order(std::list<Node *> &raw) {
    const auto isAddToJoin = [](const auto &n) {
        return n->type == '+' && n->left == nullptr;
    };

    auto it = std::find_if(raw.begin(), raw.end(), isAddToJoin);
    while (it != raw.end()) {
        auto left = it;
        --left;
        auto right = it;
        ++right;

        (*it)->left = *left;
        (*it)->right = *right;

        raw.erase(left);
        raw.erase(right);

        it = std::find_if(raw.begin(), raw.end(), isAddToJoin);
    }

    const auto isMulToJoin = [](const auto &n) {
        return n->type == '*' && n->left == nullptr;
    };

    it = std::find_if(raw.begin(), raw.end(), isMulToJoin);
    while (it != raw.end()) {
        auto left = it;
        --left;
        auto right = it;
        ++right;

        (*it)->left = *left;
        (*it)->right = *right;

        raw.erase(left);
        raw.erase(right);

        it = std::find_if(raw.begin(), raw.end(), isMulToJoin);
    }

    assert(raw.size() == 1);
    return raw.front();
}

Node *transform(const Tokenized &tokens, std::size_t &i,
                std::vector<std::unique_ptr<Node>> &all) {
    std::list<Node *> raw;
    while (i < tokens.size()) {
        const auto &t = tokens[i];
        switch (t.type) {
        case '(':
            ++i;
            raw.push_back(transform(tokens, i, all));
            assert(tokens[i].type == ')');
            break;
        case ')':
            assert(raw.size() > 0);
            return order(raw);
        case '+':
        case '*':
            all.emplace_back(std::make_unique<Node>(t.type));
            raw.push_back(all.back().get());
            break;
        case '0':
            all.emplace_back(std::make_unique<Node>(t.type, t.asNumber));
            raw.push_back(all.back().get());
            break;
        default:
            std::clog << "Oh dear..." << std::endl;
        }

        ++i;
    }

    return order(raw);
}

Number evaluate(Node *root) {
    switch (root->type) {
    case '+':
        root->value = evaluate(root->left) + evaluate(root->right);
        break;
    case '*':
        root->value = evaluate(root->left) * evaluate(root->right);
    default:
        break;
    }

    return root->value;
}

int main(int argc, char args[]) {
    std::vector<Tokenized> input;

    std::string line;
    do {
        std::getline(std::cin, line);
        if (!std::cin)
            break;

        const auto it = std::remove(line.begin(), line.end(), ' ');
        line.erase(it, line.end());

        char tmp;
        Tokenized t;
        std::istringstream ss{line};
        while (ss) {
            const auto peeked = static_cast<char>(ss.peek());
            if (peeked == '(' || peeked == ')' || peeked == '+' ||
                peeked == '*') {
                ss >> tmp;
                t.emplace_back(Token{tmp});
            } else if (peeked != EOF) {
                Number n;
                ss >> n;
                t.emplace_back(Token{'0', n});
            } else {
                break;
            }
        }
        input.emplace_back(std::move(t));
    } while (std::cin);

    std::clog << "Read " << input.size() << " lines" << std::endl;

    {
        Number sum = 0;

        for (const auto &i : input) {
            std::size_t context = 0;
            const auto result = process(i, context);
            std::clog << "result: " << result << std::endl;

            sum += result;
        }

        std::cout << "Final sum simple " << sum << std::endl;
    }

    {
        Number sum = 0;

        for (const auto &i : input) {
            std::size_t context = 0;
            std::vector<std::unique_ptr<Node>> keeper;

            const auto root = transform(i, context, keeper);
            const auto result = evaluate(root);
            std::clog << "result: " << result << std::endl;

            sum += result;
        }

        std::cout << "Final sum advanced " << sum << std::endl;
    }
    return 0;
}