#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>
using namespace std;

void insertionSort(vector<int>& a) {
    for (size_t i = 1; i < a.size(); ++i) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j-1] > key) {
            a[j] = a[j-1];
            --j;
        }
        a[j] = key;
    }
}
int main() {
    const int sizes[] = {1200, 2000, 7500};
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(-100000, 100000);
    cout << "Variant 8 - Insertion sort\n";
    for (int n : sizes) {
        vector<int> a(n);
        for (int& x : a) x = dist(rng);
        auto start = chrono::steady_clock::now();
        insertionSort(a);
        auto finish = chrono::steady_clock::now();
        cout << "N=" << n << "  t=" << fixed << setprecision(6)
             << chrono::duration<double>(finish-start).count() << " s"
             << "  sorted=" << (is_sorted(a.begin(),a.end()) ? "YES":"NO") << '\n';
    }
}
