#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>
using namespace std;

// Variant 3: Shell sort / Вариант 3: метод Шелла / 第3变体：希尔排序
void shellSort(vector<int>& a) {
    for (size_t gap = a.size() / 2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < a.size(); ++i) {
            int temp = a[i];
            size_t j = i;
            while (j >= gap && a[j - gap] > temp) {
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
    }
}

// Variant 8: Insertion sort / Вариант 8: простая вставка / 第8变体：直接插入排序
void insertionSort(vector<int>& a) {
    for (size_t i = 1; i < a.size(); ++i) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

void runVariant(int variant) {
    const vector<int> sizes = variant == 3
        ? vector<int>{2000, 5000, 6800}
        : vector<int>{1200, 2000, 7500};
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(-100000, 100000);
    cout << "\nVariant " << variant
         << (variant == 3 ? " - Shell sort" : " - Insertion sort") << '\n';
    for (int n : sizes) {
        vector<int> data(n);
        for (int& x : data) x = dist(rng);
        const auto begin = chrono::steady_clock::now();
        if (variant == 3) shellSort(data);
        else insertionSort(data);
        const auto end = chrono::steady_clock::now();
        const double seconds = chrono::duration<double>(end - begin).count();
        cout << "N=" << n << "  time=" << fixed << setprecision(6)
             << seconds << " s  sorted="
             << (is_sorted(data.begin(), data.end()) ? "YES" : "NO") << '\n';
    }
}

int main() {
    cout << "Lab 1, Part 1 / ЛР 1, часть 1 / 实验1第一部分\n";
    runVariant(3);
    runVariant(8);
    return 0;
}
