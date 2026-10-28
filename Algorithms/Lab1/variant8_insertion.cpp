// Лабораторная работа №1, часть 1. Вариант 8.
// Insertion. Самостоятельная программа C++17.
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

struct Stats {
    std::uint64_t comparisons = 0;
    std::uint64_t moves = 0;
};

// Простая вставка: вставка очередного элемента в отсортированную часть.
void sortArray(std::vector<int>& a, Stats& s) {
    for (std::size_t i = 1; i < a.size(); ++i) {
        const int key = a[i];
        ++s.moves;
        std::size_t j = i;
        while (j > 0) {
            ++s.comparisons;
            if (a[j - 1] <= key) break;
            a[j] = a[j - 1];
            ++s.moves;
            --j;
        }
        a[j] = key;
        ++s.moves;
    }
}

enum class InputKind { Random, Ascending, Descending };
const char* inputName(InputKind kind) {
    switch (kind) {
        case InputKind::Random: return "random";
        case InputKind::Ascending: return "ascending";
        default: return "descending";
    }
}

std::vector<int> makeData(int n, InputKind kind, unsigned seed) {
    std::mt19937 engine(seed);
    std::uniform_int_distribution<int> distribution(-100000, 100000);
    std::vector<int> values(static_cast<std::size_t>(n));
    for (int& value : values) value = distribution(engine);
    if (kind != InputKind::Random) {
        std::sort(values.begin(), values.end());
        if (kind == InputKind::Descending) std::reverse(values.begin(), values.end());
    }
    return values;
}

int main() {
    const int sizes[] = {1200, 2000, 7500};
    const InputKind kinds[] = {InputKind::Random, InputKind::Ascending, InputKind::Descending};
    const int experiments = 5;
    const char* csvName = "variant8_results.csv";
    std::ofstream csv(csvName);
    if (!csv) {
        std::cerr << "Cannot create CSV file: " << csvName << '\n';
        return 1;
    }
    csv << "variant,method,input,n,trial,time_us,comparisons,moves,correct\n";
    std::cout << "Laboratory work 1 / Part 1 / Variant 8\n";
    std::cout << "Insertion\n";
    std::cout << "Five trials per size and input type. Time in microseconds.\n\n";
    std::cout << std::fixed << std::setprecision(2);

    for (InputKind kind : kinds) {
        std::cout << "Input: " << inputName(kind) << '\n';
        for (int n : sizes) {
            double totalUs = 0.0;
            std::uint64_t totalComparisons = 0;
            std::uint64_t totalMoves = 0;
            for (int trial = 1; trial <= experiments; ++trial) {
                // Generation and validation are excluded from timed region.
                auto values = makeData(n, kind,
                    static_cast<unsigned>(2026 + n * 17 + trial * 31));
                auto expected = values;
                std::sort(expected.begin(), expected.end());

                Stats stats;
                const auto start = std::chrono::steady_clock::now();
                sortArray(values, stats);
                const auto stop = std::chrono::steady_clock::now();

                const bool correct = (values == expected);
                if (!correct) {
                    std::cerr << "ERROR: incorrect sort; n=" << n
                              << ", trial=" << trial << '\n';
                    return 2;
                }
                const double microseconds =
                    std::chrono::duration<double, std::micro>(stop - start).count();
                totalUs += microseconds;
                totalComparisons += stats.comparisons;
                totalMoves += stats.moves;
                csv << 8 << ",Insertion," << inputName(kind) << ','
                    << n << ',' << trial << ',' << microseconds << ','
                    << stats.comparisons << ',' << stats.moves << ",YES\n";
                std::cout << "  N=" << n << " trial=" << trial
                          << " time=" << microseconds << " us"
                          << " comparisons=" << stats.comparisons
                          << " moves=" << stats.moves << " OK\n";
            }
            std::cout << "  AVERAGE N=" << n
                      << " time=" << totalUs / experiments << " us"
                      << " comparisons=" << totalComparisons / experiments
                      << " moves=" << totalMoves / experiments << "\n\n";
        }
    }
    std::cout << "CSV saved: " << csvName << '\n';
    return 0;
}
