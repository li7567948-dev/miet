#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>
using namespace std;

void shellSort(vector<int>& a) {
    for (size_t gap = a.size()/2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < a.size(); ++i) {
            int value = a[i];
            size_t j = i;
            while (j >= gap && a[j-gap] > value) {
                a[j] = a[j-gap];
                j -= gap;
            }
            a[j] = value;
        }
    }
}
int main() {
    const int sizes[] = {2000, 5000, 6800};
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(-100000, 100000);
    cout << "Variant 3 - Shell sort\n";
    for (int n : sizes) {
        vector<int> a(n);
        for (int& x : a) x = dist(rng);
        auto start = chrono::steady_clock::now();
        shellSort(a);
        auto finish = chrono::steady_clock::now();
        cout << "N=" << n << "  t=" << fixed << setprecision(6)
             << chrono::duration<double>(finish-start).count() << " s"
             << "  sorted=" << (is_sorted(a.begin(),a.end()) ? "YES":"NO") << '\n';
    }
}
