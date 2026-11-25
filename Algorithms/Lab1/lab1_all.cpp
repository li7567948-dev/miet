#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <iomanip>
#include <fstream>
#include <stdexcept>
using namespace std;
using Clock = chrono::steady_clock;
constexpr int TRIALS=5, QUERIES=10000;
mt19937 generator(42);
struct Statistics { long long comparisons=0, moves=0; };
vector<int> generateArray(int n) {
    uniform_int_distribution<int> dist(0,999999);
    vector<int> a(n); for(int& x:a) x=dist(generator); return a;
}
// Вариант 3. Сортировка Шелла.
Statistics shellSort(vector<int>& a) {
    Statistics s; int n=static_cast<int>(a.size());
    for(int gap=n/2;gap>0;gap/=2) {
        for(int i=gap;i<n;++i) {
            int key=a[i], j=i; ++s.moves;
            while(j>=gap) {
                ++s.comparisons; if(a[j-gap]<=key) break;
                a[j]=a[j-gap]; ++s.moves; j-=gap;
            }
            a[j]=key; ++s.moves;
        }
    }
    return s;
}
// Вариант 8. Сортировка простой вставкой.
Statistics insertionSort(vector<int>& a) {
    Statistics s;
    for(int i=1;i<static_cast<int>(a.size());++i) {
        int key=a[i],j=i-1; ++s.moves;
        while(j>=0) {
            ++s.comparisons; if(a[j]<=key) break;
            a[j+1]=a[j]; ++s.moves; --j;
        }
        a[j+1]=key; ++s.moves;
    }
    return s;
}
// Вариант 6. Поиск по бору (неотрицательные целые ключи).
class Trie {
    struct Node { array<int,10> next; bool terminal=false; Node(){next.fill(-1);} };
    vector<Node> nodes{Node{}};
public:
    void insert(int value) {
        if(value<0) throw invalid_argument("Negative key");
        int current=0;
        for(char c:to_string(value)) {
            int d=c-'0';
            if(nodes[current].next[d]==-1) {
                nodes[current].next[d]=static_cast<int>(nodes.size());
                nodes.emplace_back();
            }
            current=nodes[current].next[d];
        }
        nodes[current].terminal=true;
    }
    bool search(int value,long long& steps) const {
        if(value<0) return false;
        int current=0;
        for(char c:to_string(value)) {
            ++steps; int next=nodes[current].next[c-'0'];
            if(next==-1) return false;
            current=next;
        }
        return nodes[current].terminal;
    }
};
// Измерение времени и проверка результатов сортировки.
void testSorting(bool shell) {
    vector<int> sizes=shell?vector<int>{4000,10000,13600}:vector<int>{1200,2000,7500};
    string method=shell?"Shell sort":"Insertion sort";
    string filename=shell?"shell_results.csv":"insertion_results.csv";
    ofstream file(filename); if(!file) throw runtime_error("Cannot create CSV");
    file<<"Method,N,Type,Time_s,Comparisons,Moves,Correct\n";
    array<string,3> types{"Random","Ascending","Descending"};
    for(int n:sizes) for(int type=0;type<3;++type) {
        double seconds=0; long long comparisons=0,moves=0; bool correct=true;
        for(int trial=0;trial<TRIALS;++trial) {
            auto a=generateArray(n);
            if(type==1) sort(a.begin(),a.end());
            if(type==2) sort(a.rbegin(),a.rend());
            auto expected=a; sort(expected.begin(),expected.end());
            auto start=Clock::now();
            auto stat=shell?shellSort(a):insertionSort(a);
            auto finish=Clock::now();
            seconds+=chrono::duration<double>(finish-start).count();
            comparisons+=stat.comparisons; moves+=stat.moves;
            correct=correct&&(a==expected);
        }
        cout<<fixed<<setprecision(9)<<method<<" | N = "<<n<<" | "<<types[type]<<" | t = "<<seconds/TRIALS<<" s | C = "<<double(comparisons)/TRIALS<<" | M = "<<double(moves)/TRIALS<<" | Correct: "<<(correct?"YES":"NO")<<'\n';
        file<<fixed<<setprecision(9)<<method<<','<<n<<','<<types[type]<<','<<seconds/TRIALS<<','<<double(comparisons)/TRIALS<<','<<double(moves)/TRIALS<<','<<(correct?"YES":"NO")<<'\n';
        if(!correct) throw runtime_error("Sorting verification failed");
    }
    cout<<"Saved: "<<filename<<'\n';
}
// Успешный и неуспешный поиск. Построение бора не входит во время поиска.
void testTrieSearch() {
    ofstream file("trie_results.csv"); if(!file) throw runtime_error("Cannot create CSV");
    file<<"Method,N,Type,Time_s,Steps,Correct\n";
    for(int n:{500,2500,6500}) {
        auto data=generateArray(n); Trie trie;
        for(int x:data) trie.insert(x);
        auto reference=data; sort(reference.begin(),reference.end());
        uniform_int_distribution<int> index(0,n-1), key(0,999999);
        for(int type=0;type<2;++type) {
            vector<int> queries(QUERIES);
            for(int& q:queries) {
                if(type==0) q=data[index(generator)];
                else do { q=key(generator); } while(binary_search(reference.begin(),reference.end(),q));
            }
            double seconds=0; long long totalSteps=0; bool correct=true;
            vector<unsigned char> results(QUERIES);
            for(int trial=0;trial<TRIALS;++trial) {
                long long steps=0; auto start=Clock::now();
                for(int i=0;i<QUERIES;++i) results[i]=trie.search(queries[i],steps);
                auto finish=Clock::now();
                seconds+=chrono::duration<double>(finish-start).count(); totalSteps+=steps;
                for(int i=0;i<QUERIES;++i) correct=correct&&(results[i]==(type==0));
            }
            string name=type==0?"Successful":"Unsuccessful";
            double t=seconds/(TRIALS*QUERIES), steps=double(totalSteps)/(TRIALS*QUERIES);
            cout<<fixed<<setprecision(9)<<"Trie search | N = "<<n<<" | "<<name<<" | t = "<<t<<" s | Steps = "<<steps<<" | Correct: "<<(correct?"YES":"NO")<<'\n';
            file<<fixed<<setprecision(9)<<"Trie search,"<<n<<','<<name<<','<<t<<','<<steps<<','<<(correct?"YES":"NO")<<'\n';
            if(!correct) throw runtime_error("Search verification failed");
        }
    }
    cout<<"Saved: trie_results.csv\n";
}
int main() {
    try {
        for(;;) {
            cout<<"\nLaboratory work No. 1\n1. Shell sort (Variant 3)\n2. Insertion sort (Variant 8)\n3. Trie search (Variant 6)\n4. Run all experiments\n0. Exit\nChoose: ";
            int choice; if(!(cin>>choice)) return cin.eof()?0:1;
            switch(choice) {
                case 0:return 0;
                case 1:testSorting(true);break;
                case 2:testSorting(false);break;
                case 3:testTrieSearch();break;
                case 4:testSorting(true);testSorting(false);testTrieSearch();break;
                default:cout<<"Invalid option\n";
            }
        }
    } catch(const exception& e) {cerr<<e.what()<<'\n';return 1;}
}