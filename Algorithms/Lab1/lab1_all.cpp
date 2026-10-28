#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;
using Clock = chrono::steady_clock;

void shellSort(vector<int>& a) {
    for (int gap = (int)a.size() / 2; gap > 0; gap /= 2)
        for (int i = gap; i < (int)a.size(); ++i) {
            int value = a[i], j = i;
            while (j >= gap && a[j-gap] > value) {
                a[j] = a[j-gap];
                j -= gap;
            }
            a[j] = value;
        }
}
void insertionSort(vector<int>& a) {
    for (int i = 1; i < (int)a.size(); ++i) {
        int value = a[i], j = i;
        while (j > 0 && a[j-1] > value) {
            a[j] = a[j-1];
            --j;
        }
        a[j] = value;
    }
}
vector<int> makeData(int n, unsigned seed) {
    mt19937 rng(seed);
    uniform_int_distribution<int> dist(-100000, 100000);
    vector<int> a(n);
    for (int& x : a) x = dist(rng);
    return a;
}
struct TrieNode {
    array<unique_ptr<TrieNode>, 10> next{};
    bool terminal = false;
};
class DigitTrie {
    TrieNode root;
    static string key(int x) { return to_string((long long)x + 1000000LL); }
public:
    void insert(int x) {
        TrieNode* p = &root;
        for (char c : key(x)) {
            int d = c - '0';
            if (!p->next[d]) p->next[d] = make_unique<TrieNode>();
            p = p->next[d].get();
        }
        p->terminal = true;
    }
    bool contains(int x) const {
        const TrieNode* p = &root;
        for (char c : key(x)) {
            int d = c - '0';
            if (!p->next[d]) return false;
            p = p->next[d].get();
        }
        return p->terminal;
    }
};
void runSort(int variant) {
    vector<int> sizes = variant == 3 ? vector<int>{2000,5000,6800} : vector<int>{1200,2000,7500};
    string filename = "sort_variant" + to_string(variant) + ".csv";
    ofstream csv(filename);
    csv << "variant,size,case,trial,time_us,correct\n";
    cout << "\nSORT variant " << variant << " (5 trials per case)\n";
    for (int n : sizes) for (int kind = 0; kind < 3; ++kind) {
        string label = kind == 0 ? "random" : kind == 1 ? "ascending" : "descending";
        double sum = 0;
        for (int trial = 1; trial <= 5; ++trial) {
            vector<int> a = makeData(n, 2026u + (unsigned)n + (unsigned)trial);
            if (kind != 0) {
                sort(a.begin(), a.end());
                if (kind == 2) reverse(a.begin(), a.end());
            }
            auto start = Clock::now();
            if (variant == 3) shellSort(a); else insertionSort(a);
            auto stop = Clock::now();
            double us = chrono::duration<double, micro>(stop-start).count();
            bool ok = is_sorted(a.begin(), a.end());
            csv << variant << ',' << n << ',' << label << ',' << trial << ',' << fixed << setprecision(3) << us << ',' << (ok ? "YES" : "NO") << '\n';
            sum += us;
            if (!ok) { cerr << "SORT ERROR\n"; return; }
        }
        cout << "N=" << n << " " << setw(10) << label << " average=" << fixed << setprecision(2) << sum/5 << " us  sorted=YES\n";
    }
    cout << "Saved " << filename << "\n";
}
void runSearch(int variant) {
    vector<int> sizes = variant == 3 ? vector<int>{2000,5000,6800} : vector<int>{1200,2000,7500};
    string filename = "poisk_variant" + to_string(variant) + ".csv";
    ofstream csv(filename);
    csv << "variant,size,case,trial,search_us,correct\n";
    cout << "\nPOISK variant " << variant << " (5 trials per case)\n";
    for (int n : sizes) for (int kind = 0; kind < 2; ++kind) {
        string label = kind == 0 ? "present" : "absent";
        double sum = 0;
        for (int trial = 1; trial <= 5; ++trial) {
            vector<int> a = makeData(n, 707u + (unsigned)n + (unsigned)trial);
            int query = kind == 0 ? a[n/2] : 2000000;
            bool found = false;
            if (variant == 3) {
                DigitTrie trie;
                for (int x : a) trie.insert(x);
                auto start = Clock::now();
                found = trie.contains(query);
                auto stop = Clock::now();
                double us = chrono::duration<double, micro>(stop-start).count();
                bool ok = found == (kind == 0);
                csv << variant << ',' << n << ',' << label << ',' << trial << ',' << fixed << setprecision(3) << us << ',' << (ok ? "YES" : "NO") << '\n';
                sum += us;
                if (!ok) { cerr << "SEARCH ERROR\n"; return; }
            } else {
                unordered_set<int> table(a.begin(), a.end());
                auto start = Clock::now();
                found = table.find(query) != table.end();
                auto stop = Clock::now();
                double us = chrono::duration<double, micro>(stop-start).count();
                bool ok = found == (kind == 0);
                csv << variant << ',' << n << ',' << label << ',' << trial << ',' << fixed << setprecision(3) << us << ',' << (ok ? "YES" : "NO") << '\n';
                sum += us;
                if (!ok) { cerr << "SEARCH ERROR\n"; return; }
            }
        }
        cout << "N=" << n << " " << setw(7) << label << " average=" << fixed << setprecision(3) << sum/5 << " us  correct=YES\n";
    }
    cout << "Saved " << filename << "\n";
}
int main() {
    while (true) {
        cout << "\n=== LAB 1: SORT + POISK ===\n"
             << "1 - SORT variant 3 (Shell)\n"
             << "2 - SORT variant 8 (Insertion)\n"
             << "3 - POISK variant 3 (Digit trie)\n"
             << "4 - POISK variant 8 (Hash table)\n"
             << "5 - Run all\n"
             << "0 - Exit\nChoose: ";
        int choice;
        if (!(cin >> choice)) break;
        switch (choice) {
            case 1: runSort(3); break;
            case 2: runSort(8); break;
            case 3: runSearch(3); break;
            case 4: runSearch(8); break;
            case 5: runSort(3); runSort(8); runSearch(3); runSearch(8); break;
            case 0: return 0;
            default: cout << "Unknown choice.\n";
        }
    }
}
