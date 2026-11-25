#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>
#include <limits>
#include <fstream>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using Clock = chrono::steady_clock;
using Array = vector<long double>;

// Лабораторная работа № 2. Итерация, рекурсия и остовные деревья.
// Часть 1. Формирование массива: варианты 3, 4, 6.
constexpr int RepeatCount = 1000;

// Начальные значения a_0 для каждого варианта.
long double initialTerm(int variant)
{
    switch (variant)
    {
        case 3: return 3.0L;
        case 4: return 1.0L / 3.0L;
        case 6: return 0.5L;
        default: throw invalid_argument("Unknown variant");
    }
}

// Отношение a_n / a_(n-1):
// Вариант 3: 3^(n+1) / n!, отношение 3/n.
// Вариант 4: n! / (2^n * 3^(n+1)), отношение n/6.
// Вариант 6: 3^n / (2 * n!), отношение 3/n.
long double termRatio(int variant, size_t index)
{
    if (variant == 4)
    {
        return static_cast<long double>(index) / 6.0L;
    }
    return 3.0L / static_cast<long double>(index);
}

// Итеративное заполнение массива.
void fillIterative(Array& values, int variant)
{
    if (values.empty())
    {
        return;
    }
    values[0] = initialTerm(variant);
    for (size_t index = 1; index < values.size(); ++index)
    {
        values[index] = values[index - 1] * termRatio(variant, index);
    }
}

// Рекурсивное заполнение: условие остановки и рекурсивный вызов.
void fillRecursive(Array& values, int variant, size_t index = 0)
{
    if (index == values.size())
    {
        return;
    }
    if (index == 0)
    {
        values[index] = initialTerm(variant);
    }
    else
    {
        values[index] = values[index - 1] * termRatio(variant, index);
    }
    fillRecursive(values, variant, index + 1);
}

// Независимая проверка по исходной формуле через логарифм факториала.
long double referenceTerm(int variant, size_t index)
{
    const long double n = static_cast<long double>(index);
    const long double logFactorial = lgamma(n + 1.0L);
    if (variant == 3)
    {
        return exp((n + 1.0L) * log(3.0L) - logFactorial);
    }
    if (variant == 4)
    {
        return exp(logFactorial - n * log(2.0L) - (n + 1.0L) * log(3.0L));
    }
    return exp(n * log(3.0L) - log(2.0L) - logFactorial);
}

bool verifyArray(const Array& values, int variant)
{
    for (size_t index = 0; index < values.size(); ++index)
    {
        const long double expected = referenceTerm(variant, index);
        if (!isfinite(values[index]) || abs(values[index] / expected - 1.0L) > 1e-10L)
        {
            return false;
        }
    }
    return true;
}

// Измерение среднего времени заполнения массива в микросекундах.
template <typename Function>
double measureArray(Array& values, Function fill)
{
    double total = 0.0;
    volatile long double checksum = 0.0L;
    for (int trial = 0; trial < RepeatCount; ++trial)
    {
        const auto start = Clock::now();
        fill(values);
        const auto finish = Clock::now();
        total += chrono::duration<double, micro>(finish - start).count();
        checksum = checksum + values.back();
    }
    (void)checksum;
    return total / RepeatCount;
}

void runSequence(int variant, ostream& output, ostream& csv)
{
    output << "\nSequence variant " << variant << '\n';
    for (int count : {10, 50, 100})
    {
        Array iterative(count);
        Array recursive(count);
        const double iterativeTime = measureArray(iterative, [variant](Array& a)
        {
            fillIterative(a, variant);
        });
        const double recursiveTime = measureArray(recursive, [variant](Array& a)
        {
            fillRecursive(a, variant);
        });
        const bool correct = iterative == recursive && verifyArray(iterative, variant);
        output << "N=" << count << ", iterative=" << iterativeTime
               << " us, recursive=" << recursiveTime
               << " us, check=" << (correct ? "PASS" : "FAIL") << '\n';
        csv << variant << ',' << count << ',' << iterativeTime << ','
            << recursiveTime << ',' << (correct ? "PASS" : "FAIL") << '\n';
        if (count == 10)
        {
            for (int index = 0; index < count; ++index)
            {
                output << "a[" << index << "] = " << iterative[index] << '\n';
            }
        }
        if (!correct)
        {
            throw runtime_error("Sequence verification failed");
        }
    }
}

// Часть 2. Минимальное остовное дерево неориентированного графа.
struct Edge
{
    int from;
    int to;
    int weight;
};

// Система непересекающихся множеств: сжатие путей и объединение по рангу.
class DisjointSet
{
private:
    vector<int> parent;
    vector<int> rank;

public:
    explicit DisjointSet(int count) : parent(count), rank(count, 0)
    {
        iota(parent.begin(), parent.end(), 0);
    }

    int findRoot(int vertex)
    {
        if (parent[vertex] != vertex)
        {
            parent[vertex] = findRoot(parent[vertex]);
        }
        return parent[vertex];
    }

    bool unite(int first, int second)
    {
        first = findRoot(first);
        second = findRoot(second);
        if (first == second)
        {
            return false;
        }
        if (rank[first] < rank[second])
        {
            swap(first, second);
        }
        parent[second] = first;
        if (rank[first] == rank[second])
        {
            ++rank[first];
        }
        return true;
    }
};

// Алгоритм Крускала: сортировка рёбер и исключение циклов.
vector<Edge> kruskal(int vertexCount, vector<Edge> edges)
{
    sort(edges.begin(), edges.end(), [](const Edge& first, const Edge& second)
    {
        return first.weight < second.weight;
    });
    DisjointSet components(vertexCount);
    vector<Edge> tree;
    for (const Edge& edge : edges)
    {
        if (components.unite(edge.from, edge.to))
        {
            tree.push_back(edge);
        }
        if (tree.size() == static_cast<size_t>(vertexCount - 1))
        {
            break;
        }
    }
    return tree;
}

// Алгоритм Прима: выбор минимального ребра на границе дерева.
vector<Edge> prim(int vertexCount, const vector<Edge>& edges)
{
    vector<bool> visited(vertexCount, false);
    vector<Edge> tree;
    visited[0] = true;
    while (tree.size() < static_cast<size_t>(vertexCount - 1))
    {
        const Edge* bestEdge = nullptr;
        for (const Edge& edge : edges)
        {
            const bool crossesBoundary = visited[edge.from] != visited[edge.to];
            if (crossesBoundary && (bestEdge == nullptr || edge.weight < bestEdge->weight))
            {
                bestEdge = &edge;
            }
        }
        if (bestEdge == nullptr)
        {
            break;
        }
        tree.push_back(*bestEdge);
        visited[bestEdge->from] = true;
        visited[bestEdge->to] = true;
    }
    return tree;
}

// Проверка числа рёбер и отсутствия циклов.
bool isSpanningTree(int vertexCount, const vector<Edge>& tree)
{
    if (tree.size() != static_cast<size_t>(vertexCount - 1))
    {
        return false;
    }
    DisjointSet components(vertexCount);
    for (const Edge& edge : tree)
    {
        if (!components.unite(edge.from, edge.to))
        {
            return false;
        }
    }
    return true;
}

long long printTree(const char* name, const vector<Edge>& tree, ostream& output)
{
    long long totalWeight = 0;
    output << name << '\n';
    for (const Edge& edge : tree)
    {
        output << edge.from + 1 << " - " << edge.to + 1
               << ": " << edge.weight << '\n';
        totalWeight += edge.weight;
    }
    output << "Total weight: " << totalWeight << '\n';
    return totalWeight;
}

// Сравнение результатов двух алгоритмов.
void runGraph(int vertexCount, const vector<Edge>& edges, ostream& output)
{
    const auto startKruskal = Clock::now();
    const auto kruskalTree = kruskal(vertexCount, edges);
    const auto endKruskal = Clock::now();
    const auto startPrim = Clock::now();
    const auto primTree = prim(vertexCount, edges);
    const auto endPrim = Clock::now();
    if (!isSpanningTree(vertexCount, kruskalTree) || !isSpanningTree(vertexCount, primTree))
    {
        output << "Disconnected graph: no spanning tree\n";
        return;
    }
    const long long firstWeight = printTree("Kruskal", kruskalTree, output);
    const long long secondWeight = printTree("Prim", primTree, output);
    output << "Kruskal time: " << chrono::duration<double, micro>(endKruskal - startKruskal).count() << " us\n";
    output << "Prim time: " << chrono::duration<double, micro>(endPrim - startPrim).count() << " us\n";
    output << "Equal weights: " << (firstWeight == secondWeight ? "PASS" : "FAIL") << '\n';
    if (firstWeight != secondWeight)
    {
        throw runtime_error("MST verification failed");
    }
}

// Ввод графа варианта. Нумерация вершин начинается с единицы.
void inputGraph(int variant, ostream& output)
{
    int vertexCount = 0;
    int edgeCount = 0;
    cout << "Graph variant " << variant << ": vertices, edges: ";
    if (!(cin >> vertexCount >> edgeCount) || vertexCount < 1 || edgeCount < 0)
    {
        throw runtime_error("Invalid graph size");
    }
    vector<Edge> edges;
    cout << "Enter edges (u v weight):\n";
    for (int index = 0; index < edgeCount; ++index)
    {
        Edge edge{};
        if (!(cin >> edge.from >> edge.to >> edge.weight) ||
            edge.from < 1 || edge.from > vertexCount || edge.to < 1 || edge.to > vertexCount)
        {
            throw runtime_error("Invalid edge");
        }
        --edge.from;
        --edge.to;
        edges.push_back(edge);
    }
    output << "Graph variant " << variant << " (user input)\n";
    runGraph(vertexCount, edges, output);
}

void printMenu()
{
    cout << "\nLaboratory work No. 2\n"
         << "1 - Sequence variant 3\n"
         << "2 - Sequence variant 4\n"
         << "3 - Sequence variant 6\n"
         << "4 - Graph variant 4 (manual input)\n"
         << "5 - Graph variant 6 (manual input)\n"
         << "6 - All sequences and sample graph\n"
         << "0 - Exit\nChoice: ";
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    cout << setprecision(10);
    ofstream csv("lab2_sequence_results.csv");
    if (!csv)
    {
        cerr << "Cannot create CSV file\n";
        return 1;
    }
    csv << "variant,N,iterative_us,recursive_us,check\n";
    try
    {
        while (true)
        {
            printMenu();
            int choice = 0;
            if (!(cin >> choice) || choice == 0)
            {
                break;
            }
            switch (choice)
            {
                case 1: runSequence(3, cout, csv); break;
                case 2: runSequence(4, cout, csv); break;
                case 3: runSequence(6, cout, csv); break;
                case 4: inputGraph(4, cout); break;
                case 5: inputGraph(6, cout); break;
                case 6:
                {
                    for (int variant : {3, 4, 6})
                    {
                        runSequence(variant, cout, csv);
                    }
                    cout << "\nSample graph (not a textbook variant)\n";
                    runGraph(4, {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {0, 3, 8}, {0, 2, 5}}, cout);
                    break;
                }
                default: cout << "Invalid menu choice\n";
            }
            csv.flush();
        }
    }
    catch (const exception& error)
    {
        cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}