// Лабораторная работа № 3. Кратчайшие пути на графах.
// Часть 1, лабораторная работа № 5: задания 1 и 2.
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

using Distance = long long;
constexpr Distance INF = std::numeric_limits<Distance>::max() / 4;

struct Edge
{
    int from;
    int to;
    int weight;
};

struct Graph
{
    int vertexCount;
    bool directed;
    std::vector<Edge> edges;
};

struct Result
{
    std::vector<Distance> distance;
    std::vector<int> path;
};

// Для неориентированного графа каждое ребро задаёт две дуги.
std::vector<Edge> arcs(const Graph& graph)
{
    std::vector<Edge> result = graph.edges;
    if (!graph.directed)
    {
        for (const Edge& edge : graph.edges)
        {
            result.push_back({edge.to, edge.from, edge.weight});
        }
    }
    return result;
}

// Динамическое программирование: d[k][v] — минимум для путей
// из начальной вершины в v, содержащих не более k дуг.
Result dynamicProgramming(const Graph& graph, int source, int target,
                          std::ostream* trace = nullptr)
{
    const int n = graph.vertexCount;
    const auto edges = arcs(graph);
    std::vector<std::vector<Distance>> d(n, std::vector<Distance>(n + 1, INF));
    std::vector<std::vector<int>> parent(n, std::vector<int>(n + 1, -1));
    d[0][source] = 0;
    for (int k = 1; k < n; ++k)
    {
        // Сохраняем пути, использующие меньше k дуг.
        d[k] = d[k - 1];
        for (const Edge& edge : edges)
        {
            if (d[k - 1][edge.from] == INF)
            {
                continue;
            }
            const Distance candidate = d[k - 1][edge.from] + edge.weight;
            if (candidate < d[k][edge.to])
            {
                d[k][edge.to] = candidate;
                parent[k][edge.to] = edge.from;
            }
        }
        if (trace)
        {
            *trace << "k=" << k << ": ";
            for (int v = 1; v <= n; ++v)
            {
                if (d[k][v] == INF) *trace << "INF ";
                else *trace << d[k][v] << ' ';
            }
            *trace << '\n';
        }
    }
    // Проверяем отсутствие достижимого отрицательного цикла.
    for (const Edge& edge : edges)
    {
        if (d.back()[edge.from] != INF &&
            d.back()[edge.from] + edge.weight < d.back()[edge.to])
        {
            throw std::runtime_error("Reachable negative cycle");
        }
    }
    std::vector<int> path;
    if (d.back()[target] != INF)
    {
        int vertex = target;
        path.push_back(vertex);
        for (int k = n - 1; k > 0 && vertex != source; --k)
        {
            if (parent[k][vertex] != -1)
            {
                vertex = parent[k][vertex];
                path.push_back(vertex);
            }
        }
        if (vertex != source) throw std::logic_error("Invalid path");
        std::reverse(path.begin(), path.end());
    }
    return {d.back(), path};
}

// Алгоритм Дейкстры: выбираем непосещённую вершину
// с минимальной временной меткой и уточняем расстояния.
Result dijkstra(const Graph& graph, int source, int target,
                std::ostream* trace = nullptr)
{
    const int n = graph.vertexCount;
    std::vector<std::vector<Edge>> adjacency(n + 1);
    for (const Edge& edge : arcs(graph))
    {
        if (edge.weight < 0) throw std::invalid_argument("Negative edge");
        adjacency[edge.from].push_back(edge);
    }
    std::vector<Distance> distance(n + 1, INF);
    std::vector<int> parent(n + 1, -1);
    std::vector<bool> visited(n + 1, false);
    distance[source] = 0;
    for (int step = 0; step < n; ++step)
    {
        int current = -1;
        for (int v = 1; v <= n; ++v)
        {
            if (!visited[v] && (current == -1 || distance[v] < distance[current]))
            {
                current = v;
            }
        }
        if (current == -1 || distance[current] == INF) break;
        visited[current] = true;
        for (const Edge& edge : adjacency[current])
        {
            const Distance candidate = distance[current] + edge.weight;
            if (!visited[edge.to] && candidate < distance[edge.to])
            {
                distance[edge.to] = candidate;
                parent[edge.to] = current;
            }
        }
        if (trace)
        {
            *trace << "step=" << step + 1 << ", vertex=" << current << ": ";
            for (int v = 1; v <= n; ++v)
            {
                if (distance[v] == INF) *trace << "INF ";
                else *trace << distance[v] << ' ';
            }
            *trace << '\n';
        }
    }
    std::vector<int> path;
    if (distance[target] != INF)
    {
        for (int v = target; v != -1; v = parent[v])
        {
            path.push_back(v);
        }
        std::reverse(path.begin(), path.end());
    }
    return {distance, path};
}

// Независимая проверка расстояний алгоритмом Флойда.
std::vector<std::vector<Distance>> floyd(const Graph& graph)
{
    const int n = graph.vertexCount;
    std::vector<std::vector<Distance>> d(n + 1, std::vector<Distance>(n + 1, INF));
    for (int v = 1; v <= n; ++v) d[v][v] = 0;
    for (const Edge& edge : arcs(graph))
    {
        d[edge.from][edge.to] = std::min(d[edge.from][edge.to], Distance(edge.weight));
    }
    for (int k = 1; k <= n; ++k)
    {
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (d[i][k] != INF && d[k][j] != INF)
                {
                    d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
                }
            }
        }
    }
    return d;
}

// Проверяем не только длину, но и существование всех дуг пути.
bool verifyPath(const Graph& graph, const Result& result, int source, int target)
{
    if (result.distance[target] == INF) return result.path.empty();
    if (result.path.empty() || result.path.front() != source || result.path.back() != target)
    {
        return false;
    }
    const auto edges = arcs(graph);
    Distance sum = 0;
    for (std::size_t i = 1; i < result.path.size(); ++i)
    {
        Distance weight = INF;
        for (const Edge& edge : edges)
        {
            if (edge.from == result.path[i - 1] && edge.to == result.path[i])
            {
                weight = std::min(weight, Distance(edge.weight));
            }
        }
        if (weight == INF) return false;
        sum += weight;
    }
    return sum == result.distance[target];
}

// Вывод параметров графа, шагов, маршрута и времени вычисления.
void experiment(const Graph& graph, int number, int source, int target,
                std::ostream& output, std::ostream& csv)
{
    using Clock = std::chrono::steady_clock;
    const auto start = Clock::now();
    const Result result = graph.directed ? dynamicProgramming(graph, source, target)
                                         : dijkstra(graph, source, target);
    const double elapsed = std::chrono::duration<double, std::micro>(Clock::now() - start).count();
    const auto reference = floyd(graph);
    bool passed = verifyPath(graph, result, source, target);
    for (int v = 1; v <= graph.vertexCount; ++v)
    {
        passed = passed && result.distance[v] == reference[source][v];
    }
    output << "\nTask " << (graph.directed ? 1 : 2) << ", graph " << number
           << ", V=" << graph.vertexCount << ", E=" << graph.edges.size()
           << ", " << source << " -> " << target << '\n';
    if (graph.directed) dynamicProgramming(graph, source, target, &output);
    else dijkstra(graph, source, target, &output);
    output << "Path: ";
    for (std::size_t i = 0; i < result.path.size(); ++i)
    {
        if (i) output << " -> ";
        output << result.path[i];
    }
    const std::string length = result.distance[target] == INF ? "INF" : std::to_string(result.distance[target]);
    output << "\nDistance: " << length << "\nTime (us): " << elapsed
           << "\nCheck: " << (passed ? "PASS" : "FAIL") << '\n';
    csv << (graph.directed ? 1 : 2) << ',' << number << ',' << graph.vertexCount
        << ',' << graph.edges.size() << ',' << source << ',' << target << ','
        << length << ',' << elapsed << ',' << (passed ? "PASS" : "FAIL") << '\n';
    if (!passed) throw std::logic_error("Verification failed");
}

// Три ориентированных графа: рисунки вариантов 3–4, 9–10, 11–12.
std::vector<Graph> directedGraphs()
{
    return {
        {7, true, {{1,2,1},{2,3,2},{3,6,3},{6,7,4},{1,4,8},{4,5,9},{5,7,6},{1,5,25},{1,7,20},{2,7,15}}},
        {7, true, {{1,3,11},{1,2,16},{3,2,2},{2,5,2},{3,5,5},{2,4,4},{5,4,3},{5,7,6},{4,7,1},{4,6,9},{6,7,10}}},
        {7, true, {{1,2,8},{2,3,15},{3,4,2},{1,5,11},{5,2,3},{5,3,11},{2,6,9},{5,6,10},{6,4,1},{3,7,1},{4,7,6},{6,7,3}}}
    };
}

// Три неориентированных графа: рисунки вариантов 3–4, 5–6, 7–8.
std::vector<Graph> undirectedGraphs()
{
    return {
        {8, false, {{1,2,18},{2,3,1},{1,4,4},{4,6,5},{6,7,5},{7,8,10},{3,5,13},{5,8,9},{2,5,3},{2,8,1},{1,7,9},{1,8,22},{4,7,7},{7,5,2}}},
        {8, false, {{1,2,6},{2,3,9},{3,4,12},{1,5,2},{5,2,1},{5,6,7},{2,6,5},{2,7,3},{3,7,8},{4,7,3},{4,8,4},{6,7,4},{7,8,16}}},
        {8, false, {{1,2,8},{1,4,5},{1,6,9},{2,4,1},{4,6,3},{2,3,2},{4,5,9},{6,7,12},{3,5,3},{5,7,1},{3,8,10},{5,8,6},{7,8,7}}}
    };
}

int main()
{
    try
    {
        std::ofstream csv("lab3_results.csv");
        if (!csv) throw std::runtime_error("Cannot open CSV");
        csv << "Task,Graph,Vertices,Edges,Source,Target,Distance,Time_us,Check\n";
        std::cout << std::fixed << std::setprecision(6);
        csv << std::fixed << std::setprecision(6);
        std::cout << "Laboratory work 3: shortest paths\n";
        std::cout << "1 - Dynamic programming\n2 - Dijkstra\n3 - All experiments\n0 - Exit\n";
        int choice = 0;
        while (std::cout << "Choice: " && std::cin >> choice && choice != 0)
        {
            if (choice == 1 || choice == 3)
            {
                const auto graphs = directedGraphs();
                for (std::size_t i = 0; i < graphs.size(); ++i)
                {
                    experiment(graphs[i], int(i + 1), 1, 7, std::cout, csv);
                }
            }
            if (choice == 2 || choice == 3)
            {
                const auto graphs = undirectedGraphs();
                for (std::size_t i = 0; i < graphs.size(); ++i)
                {
                    experiment(graphs[i], int(i + 1), 1, 8, std::cout, csv);
                    experiment(graphs[i], int(i + 1), 2, 7, std::cout, csv);
                    experiment(graphs[i], int(i + 1), 3, 6, std::cout, csv);
                }
            }
            if (choice < 1 || choice > 3) std::cout << "Invalid choice\n";
            csv.flush();
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}