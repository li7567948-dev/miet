// Лабораторная работа 1 / POISK / вариант 8.
// Хеширование: собственная таблица с цепочками (separate chaining).
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

class HashTable {
    std::vector<std::vector<int>> buckets;
    std::size_t index(int value) const {
        return static_cast<std::size_t>(static_cast<unsigned>(value)) % buckets.size();
    }
public:
    explicit HashTable(std::size_t expected)
        : buckets(expected * 2 + 1) {}
    void insert(int value) {
        auto &bucket = buckets[index(value)];
        if (std::find(bucket.begin(), bucket.end(), value) == bucket.end())
            bucket.push_back(value);
    }
    bool contains(int value) const {
        const auto &bucket = buckets[index(value)];
        return std::find(bucket.begin(), bucket.end(), value) != bucket.end();
    }
};
int main() {
    const int sizes[] = {1200, 2000, 7500};
    std::ofstream csv("poisk_variant8_results.csv");
    if (!csv) return 1;
    csv << "variant,method,n,trial,build_us,search_us,queries,found,correct\n";
    std::cout << "POISK variant 8: hash table search\n";
    for (int n : sizes) {
        for (int trial = 1; trial <= 5; ++trial) {
            const auto data = generate(n, 2026 + n + trial);
            HashTable table(data.size());
            auto t0 = Clock::now();
            for (int v : data) table.insert(v);
            auto t1 = Clock::now();
            std::vector<int> queries;
            queries.reserve(2 * n);
            for (int v : data) queries.push_back(v);
            for (int i = 0; i < n; ++i) queries.push_back(1000000 + i);
            std::size_t found = 0;
            auto t2 = Clock::now();
            for (int key : queries) if (table.contains(key)) ++found;
            auto t3 = Clock::now();
            const bool correct = found == static_cast<std::size_t>(n);
            const double buildUs = std::chrono::duration<double, std::micro>(t1-t0).count();
            const double searchUs = std::chrono::duration<double, std::micro>(t3-t2).count();
            csv << "8,hash," << n << ',' << trial << ',' << buildUs << ','
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
