// Лабораторная работа 1 / POISK / вариант 3.
// Поиск по бору (цифровое префиксное дерево для целых чисел).
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <string>
using Clock = std::chrono::steady_clock;
std::vector<int> generate(int n, int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 999999);
    std::vector<int> a(n);
    for (int &v : a) v = dist(rng);
    return a;
}

#include <array>
struct Node {
    std::array<int, 10> next;
    bool terminal;
    Node() : terminal(false) { next.fill(-1); }
};
class Trie {
    std::vector<Node> nodes;
public:
    Trie() { nodes.emplace_back(); }
    void insert(int value) {
        const std::string s = std::to_string(value);
        int current = 0;
        for (char ch : s) {
            const int digit = ch - '0';
            if (nodes[current].next[digit] == -1) {
                const int newNode = static_cast<int>(nodes.size());
                nodes[current].next[digit] = newNode;
                nodes.emplace_back();
            }
            current = nodes[current].next[digit];
        }
        nodes[current].terminal = true;
    }
    bool contains(int value) const {
        const std::string s = std::to_string(value);
        int current = 0;
        for (char ch : s) {
            const int digit = ch - '0';
            current = nodes[current].next[digit];
            if (current == -1) return false;
        }
        return nodes[current].terminal;
    }
};
int main() {
    const int sizes[] = {2000, 5000, 6800};
    std::ofstream csv("poisk_variant3_results.csv");
    if (!csv) return 1;
    csv << "variant,method,n,trial,build_us,search_us,queries,found,correct\n";
    std::cout << "POISK variant 3: trie search\n";
    for (int n : sizes) {
        for (int trial = 1; trial <= 5; ++trial) {
            const auto data = generate(n, 2026 + n + trial);
            Trie trie;
            auto t0 = Clock::now();
            for (int v : data) trie.insert(v);
            auto t1 = Clock::now();
            std::vector<int> queries;
            queries.reserve(2 * n);
            for (int v : data) queries.push_back(v);
            for (int i = 0; i < n; ++i) queries.push_back(1000000 + i);
            std::size_t found = 0;
            auto t2 = Clock::now();
            for (int key : queries) if (trie.contains(key)) ++found;
            auto t3 = Clock::now();
            const bool correct = found == static_cast<std::size_t>(n);
            const double buildUs = std::chrono::duration<double, std::micro>(t1-t0).count();
            const double searchUs = std::chrono::duration<double, std::micro>(t3-t2).count();
            csv << "3,trie," << n << ',' << trial << ',' << buildUs << ','
                << searchUs << ',' << queries.size() << ',' << found << ','
                << (correct ? "YES" : "NO") << '\n';
            std::cout << "N=" << n << " trial=" << trial
                      << " build_us=" << buildUs << " search_us=" << searchUs
                      << " queries=" << queries.size() << " found=" << found
                      << " correct=" << (correct ? "YES" : "NO") << '\n';
            if (!correct) return 2;
        }
    }
}
