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
