#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <limits>

using Matrix = std::vector<std::vector<int>>;
constexpr int INF = 100000000;

// Результат решения задачи коммивояжера.
struct Tour
{
    int cost = INF;
    std::vector<int> path;
    int nodes = 0;
    int pruned = 0;
};

// Узел дерева решений: активные строки и столбцы сохраняют номера вершин.
struct SearchNode
{
    Matrix matrix;
    std::vector<int> rows;
    std::vector<int> columns;
    std::vector<int> successor;
    int bound = 0;
    int selected = 0;
};

// Приведение по строкам и столбцам. INF означает запрещенную дугу.
bool reduceMatrix(SearchNode& node)
{
    for (int row : node.rows)
    {
        int minimum = INF;
        for (int column : node.columns)
        {
            minimum = std::min(minimum, node.matrix[row][column]);
        }
        if (minimum == INF)
        {
            return false;
        }
        node.bound += minimum;
        for (int column : node.columns)
        {
            if (node.matrix[row][column] != INF)
            {
                node.matrix[row][column] -= minimum;
            }
        }
    }
    for (int column : node.columns)
    {
        int minimum = INF;
        for (int row : node.rows)
        {
            minimum = std::min(minimum, node.matrix[row][column]);
        }
        if (minimum == INF)
        {
            return false;
        }
        node.bound += minimum;
        for (int row : node.rows)
        {
            if (node.matrix[row][column] != INF)
            {
                node.matrix[row][column] -= minimum;
            }
        }
    }
    return true;
}

// Выбор самого тяжелого нуля: штраф равен сумме двух минимумов.
std::pair<int, int> chooseZero(const SearchNode& node)
{
    int maximumPenalty = -1;
    std::pair<int, int> chosen{-1, -1};
    for (int row : node.rows)
    {
        for (int column : node.columns)
        {
            if (node.matrix[row][column] != 0)
            {
                continue;
            }
            int rowMinimum = INF;
            int columnMinimum = INF;
            for (int other : node.columns)
            {
                if (other != column)
                {
                    rowMinimum = std::min(rowMinimum, node.matrix[row][other]);
                }
            }
            for (int other : node.rows)
            {
                if (other != row)
                {
                    columnMinimum = std::min(columnMinimum, node.matrix[other][column]);
                }
            }
            int penalty = rowMinimum + columnMinimum;
            std::cout << "Zero (" << row + 1 << "," << column + 1
                      << "): penalty=" << penalty << '\n';
            if (penalty > maximumPenalty)
            {
                maximumPenalty = penalty;
                chosen = {row, column};
            }
        }
    }
    return chosen;
}

// Исключение преждевременного замыкания каждой цепочки.
void forbidSubtours(SearchNode& node)
{
    int n = static_cast<int>(node.successor.size());
    std::vector<bool> hasPredecessor(n, false);
    for (int next : node.successor)
    {
        if (next >= 0)
        {
            hasPredecessor[next] = true;
        }
    }
    for (int start = 0; start < n; ++start)
    {
        if (hasPredecessor[start])
        {
            continue;
        }
        int end = start;
        int length = 1;
        while (node.successor[end] >= 0)
        {
            end = node.successor[end];
            ++length;
        }
        if (length < n)
        {
            node.matrix[end][start] = INF;
        }
    }
}

// Ветвление по включению и исключению выбранной дуги.
void searchTour(const Matrix& weights, SearchNode node, Tour& best)
{
    ++best.nodes;
    if (!reduceMatrix(node) || node.bound >= best.cost)
    {
        ++best.pruned;
        return;
    }
    std::cout << "Node " << best.nodes << ": lower bound=" << node.bound << '\n';
    if (node.rows.empty())
    {
        std::vector<int> path{0};
        int current = 0;
        int cost = 0;
        for (std::size_t i = 0; i < weights.size(); ++i)
        {
            int next = node.successor[current];
            if (next < 0 || (next == 0 && i + 1 != weights.size()))
            {
                return;
            }
            cost += weights[current][next];
            path.push_back(next);
            current = next;
        }
        if (current == 0 && cost < best.cost)
        {
            best.cost = cost;
            best.path = path;
            std::cout << "New record: " << cost << '\n';
        }
        return;
    }
    for (int row : node.rows)
    {
        for (int column : node.columns)
        {
            if (node.matrix[row][column] == INF)
            {
                std::cout << std::setw(5) << "INF";
            }
            else
            {
                std::cout << std::setw(5) << node.matrix[row][column];
            }
        }
        std::cout << '\n';
    }
    auto [from, to] = chooseZero(node);
    if (from < 0)
    {
        return;
    }
    std::cout << "Include " << from + 1 << " -> " << to + 1 << '\n';
    SearchNode included = node;
    included.successor[from] = to;
    ++included.selected;
    included.rows.erase(std::find(included.rows.begin(), included.rows.end(), from));
    included.columns.erase(std::find(included.columns.begin(), included.columns.end(), to));
    forbidSubtours(included);
    searchTour(weights, included, best);
    std::cout << "Exclude " << from + 1 << " -> " << to + 1 << '\n';
    node.matrix[from][to] = INF;
    searchTour(weights, node, best);
}

Tour solveTour(const Matrix& weights)
{
    SearchNode root;
    root.matrix = weights;
    root.rows.resize(weights.size());
    std::iota(root.rows.begin(), root.rows.end(), 0);
    root.columns = root.rows;
    root.successor.assign(weights.size(), -1);
    Tour result;
    searchTour(weights, root, result);
    return result;
}

// Независимая проверка полным перебором перестановок.
int exhaustiveCost(const Matrix& weights)
{
    std::vector<int> order(weights.size() - 1);
    std::iota(order.begin(), order.end(), 1);
    int best = INF;
    do
    {
        int previous = 0;
        int cost = 0;
        for (int next : order)
        {
            if (weights[previous][next] == INF)
            {
                cost = INF;
                break;
            }
            cost += weights[previous][next];
            previous = next;
        }
        if (cost != INF && weights[previous][0] != INF)
        {
            best = std::min(best, cost + weights[previous][0]);
        }
    } while (std::next_permutation(order.begin(), order.end()));
    return best;
}

// Печать исходной матрицы, маршрута и суммы весов.
void runTour(const std::string& name, const Matrix& weights)
{
    std::cout << "\n" << name << "\n";
    int missing = 0;
    for (std::size_t i = 0; i < weights.size(); ++i)
    {
        for (std::size_t j = 0; j < weights.size(); ++j)
        {
            if (weights[i][j] == INF)
            {
                std::cout << std::setw(5) << "INF";
                if (i != j)
                {
                    ++missing;
                }
            }
            else
            {
                std::cout << std::setw(5) << weights[i][j];
            }
        }
        std::cout << '\n';
    }
    auto start = std::chrono::steady_clock::now();
    Tour result = solveTour(weights);
    auto end = std::chrono::steady_clock::now();
    std::cout << "N = " << weights.size() << ", removed arcs = " << missing << '\n';
    if (result.cost == INF)
    {
        std::cout << "No Hamiltonian cycle\n";
    }
    else
    {
        std::cout << "Route: ";
        for (std::size_t i = 0; i < result.path.size(); ++i)
        {
            std::cout << (i ? " -> " : "") << result.path[i] + 1;
        }
        std::cout << "\nCost: ";
        for (std::size_t i = 1; i < result.path.size(); ++i)
        {
            std::cout << (i == 1 ? "" : " + ")
                      << weights[result.path[i - 1]][result.path[i]];
        }
        std::cout << " = " << result.cost << '\n';
    }
    bool correct = result.cost == exhaustiveCost(weights);
    std::cout << "Nodes: " << result.nodes << ", pruned: " << result.pruned
              << "\nTime (us): " << std::chrono::duration<double, std::micro>(end - start).count()
              << "\nVerification: " << (correct ? "PASS" : "FAIL") << '\n';
    if (!correct)
    {
        throw std::runtime_error("Tour verification failed");
    }
}

// Универсальная машина Тьюринга: команда определяется состоянием и символом.
struct Transition
{
    std::string nextState;
    char write;
    int move;
};
using Rules = std::map<std::pair<std::string, char>, Transition>;

std::string simulate(const Rules& rules, const std::string& input, bool trace)
{
    std::map<int, char> tape;
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        tape[static_cast<int>(i)] = input[i];
    }
    int head = 0;
    std::string state = "q0";
    for (int step = 0; state != "halt"; ++step)
    {
        if (step >= 100000)
        {
            throw std::runtime_error("Step limit exceeded");
        }
        char read = tape.count(head) ? tape.at(head) : '_';
        auto rule = rules.find({state, read});
        if (rule == rules.end())
        {
            throw std::runtime_error("Undefined transition");
        }
        const Transition& command = rule->second;
        if (trace)
        {
            std::cout << step << ": " << state << ", head=" << head
                      << ", read=" << read << " -> " << command.nextState
                      << ", write=" << command.write << ", move=" << command.move << '\n';
        }
        if (command.write == '_')
        {
            tape.erase(head);
        }
        else
        {
            tape[head] = command.write;
        }
        head += command.move;
        state = command.nextState;
    }
    std::string output;
    if (!tape.empty())
    {
        for (int i = tape.begin()->first; i <= tape.rbegin()->first; ++i)
        {
            output += tape.count(i) ? tape.at(i) : '_';
        }
    }
    return output;
}

// Вариант 2: десятичная цифра преобразуется в унарную запись.
Rules decimalToUnary()
{
    Rules rules;
    for (int digit = 0; digit <= 9; ++digit)
    {
        rules[{"q0", static_cast<char>('0' + digit)}] =
            {digit == 0 ? "halt" : "write" + std::to_string(digit), '_', 0};
        if (digit > 0)
        {
            rules[{"write" + std::to_string(digit), '_'}] =
                {digit == 1 ? "halt" : "write" + std::to_string(digit - 1), '1', 1};
        }
    }
    return rules;
}

// Вариант 3: унарная запись преобразуется в десятичную цифру.
Rules unaryToDecimal()
{
    Rules rules;
    for (int count = 0; count <= 9; ++count)
    {
        std::string state = count == 0 ? "q0" : "count" + std::to_string(count);
        if (count < 9)
        {
            rules[{state, '1'}] = {"count" + std::to_string(count + 1), '_', 1};
        }
        rules[{state, '_'}] = {"halt", static_cast<char>('0' + count), 0};
    }
    return rules;
}

// Вариант 5: копирование двоичного слова с разделителем '*'.
Rules copyBinary()
{
    Rules rules;
    for (char bit : std::string("01"))
    {
        rules[{"q0", bit}] = {"q0", bit, 1};
        rules[{"pick", bit}] = {"carry" + std::string(1, bit), bit == '0' ? 'X' : 'Y', 1};
        for (char symbol : std::string("01XY*"))
        {
            rules[{"carry" + std::string(1, bit), symbol}] = {"carry" + std::string(1, bit), symbol, 1};
        }
        rules[{"carry" + std::string(1, bit), '_'}] = {"back", bit, -1};
    }
    rules[{"q0", '_'}] = {"back", '*', -1};
    for (char symbol : std::string("01XY*"))
    {
        rules[{"back", symbol}] = {"back", symbol, -1};
    }
    rules[{"back", '_'}] = {"pick", '_', 1};
    rules[{"pick", 'X'}] = {"pick", 'X', 1};
    rules[{"pick", 'Y'}] = {"pick", 'Y', 1};
    rules[{"pick", '*'}] = {"restore", '*', -1};
    rules[{"restore", 'X'}] = {"restore", '0', -1};
    rules[{"restore", 'Y'}] = {"restore", '1', -1};
    rules[{"restore", '_'}] = {"halt", '_', 1};
    return rules;
}

// Вариант 1: инверсия исходного слова и его зеркальная копия.
// Вариант 5: инверсия и прямая копия (как в примерах пособия).
Rules textbookBinary(bool mirror)
{
    Rules rules;
    for (char bit : std::string("01"))
    {
        char inverse = bit == '0' ? '1' : '0';
        rules[{"q0", bit}] = {"q0", inverse, 1};
        std::string carry = "carry" + std::string(1, bit);
        rules[{"pick", bit}] = {carry, bit == '0' ? 'X' : 'Y', 1};
        for (char symbol : std::string("01XY*"))
        {
            rules[{carry, symbol}] = {carry, symbol, 1};
        }
        rules[{carry, '_'}] = {"return", bit, -1};
    }
    rules[{"q0", '_'}] = {mirror ? "pick" : "back", '*', -1};
    for (char symbol : std::string("01XY*"))
    {
        rules[{"back", symbol}] = {"back", symbol, -1};
    }
    rules[{"back", '_'}] = {"pick", '_', 1};
    int direction = mirror ? -1 : 1;
    for (char symbol : std::string("XY"))
    {
        rules[{"pick", symbol}] = {"pick", symbol, direction};
    }
    for (char symbol : std::string("01"))
    {
        rules[{"return", symbol}] = {"return", symbol, -1};
    }
    rules[{"return", '*'}] = {mirror ? "pick" : "back", '*', -1};
    rules[{"pick", mirror ? '_' : '*'}] = {"restore", mirror ? '_' : '*', direction * -1};
    rules[{"restore", 'X'}] = {"restore", '0', mirror ? 1 : -1};
    rules[{"restore", 'Y'}] = {"restore", '1', mirror ? 1 : -1};
    rules[{"restore", mirror ? '*' : '_'}] = {"halt", mirror ? '*' : '_', 0};
    return rules;
}

// Вариант 4: после каждой тройки сдвигаем остаток слова вправо.
Rules groupBinary()
{
    Rules rules;
    for (char bit : std::string("01"))
    {
        rules[{"q0", bit}] = {"count1", bit, 1};
        rules[{"count1", bit}] = {"count2", bit, 1};
        rules[{"count2", bit}] = {"boundary", bit, 1};
        rules[{"boundary", bit}] = {"shift" + std::string(1, bit), '*', 1};
        for (char carried : std::string("01"))
        {
            rules[{"shift" + std::string(1, carried), bit}] =
                {"shift" + std::string(1, bit), carried, 1};
        }
        rules[{"shift" + std::string(1, bit), '_'}] = {"rewind", bit, -1};
        rules[{"rewind", bit}] = {"rewind", bit, -1};
    }
    rules[{"rewind", '*'}] = {"q0", '*', 1};
    for (const std::string state : {"q0", "count1", "count2", "boundary"})
    {
        rules[{state, '_'}] = {"halt", '_', 0};
    }
    return rules;
}

// Дополнительный вариант 6 из пособия: удалить одну палочку.
Rules decrementUnary()
{
    Rules rules;
    rules[{"q0", '1'}] = {"q0", '1', 1};
    rules[{"q0", '_'}] = {"erase", '_', -1};
    rules[{"erase", '1'}] = {"halt", '_', 0};
    return rules;
}

// Функциональная таблица и список дуг функциональной диаграммы.
void printRules(const Rules& rules)
{
    std::cout << "State | Read | Next | Write | Move\n";
    for (const auto& entry : rules)
    {
        const auto& key = entry.first;
        const auto& rule = entry.second;
        std::cout << key.first << " | " << key.second << " | " << rule.nextState
                  << " | " << rule.write << " | " << rule.move << '\n';
    }
}

void testMachine(const std::string& name, const Rules& rules,
                 const std::string& input, const std::string& expected, bool trace)
{
    std::cout << "\n" << name << ": input=" << input << '\n';
    std::string result = simulate(rules, input, trace);
    std::cout << "Output: " << result << "\nVerification: "
              << (result == expected ? "PASS" : "FAIL") << '\n';
    if (result != expected)
    {
        throw std::runtime_error("Machine verification failed");
    }
}

void runTuring()
{
    testMachine("Decimal to unary", decimalToUnary(), "5", "11111", true);
    testMachine("Unary to decimal", unaryToDecimal(), "11111", "5", true);
    testMachine("Variant 1", textbookBinary(true), "01101", "10010*01001", true);
    testMachine("Variant 4", groupBinary(), "01101011010", "011*010*110*10", true);
    testMachine("Variant 5", textbookBinary(false), "01101", "10010*10010", true);
    testMachine("Literal binary copy", copyBinary(), "01101", "01101*01101", true);
    testMachine("Extra variant 6", decrementUnary(), "111", "11", true);
    for (const Rules& rules : {textbookBinary(true), decimalToUnary(), unaryToDecimal(),
                               groupBinary(), textbookBinary(false), decrementUnary()})
    {
        printRules(rules);
    }
    // Проверка всех двоичных слов длиной от 1 до 6.
    for (int length = 1; length <= 6; ++length)
    {
        for (int mask = 0; mask < (1 << length); ++mask)
        {
            std::string input;
            std::string inverse;
            std::string grouped;
            for (int i = 0; i < length; ++i)
            {
                char bit = (mask & (1 << i)) ? '1' : '0';
                input += bit;
                inverse += bit == '0' ? '1' : '0';
                if (i > 0 && i % 3 == 0)
                {
                    grouped += '*';
                }
                grouped += bit;
            }
            std::string reversed(inverse.rbegin(), inverse.rend());
            testMachine("Mirror", textbookBinary(true), input, inverse + "*" + reversed, false);
            testMachine("Grouping", groupBinary(), input, grouped, false);
            testMachine("Copy", textbookBinary(false), input, inverse + "*" + inverse, false);
        }
    }
    for (int digit = 0; digit <= 9; ++digit)
    {
        std::string decimal = std::to_string(digit);
        std::string unary(static_cast<std::size_t>(digit), '1');
        testMachine("Decimal to unary", decimalToUnary(), decimal, unary, false);
        testMachine("Unary to decimal", unaryToDecimal(), unary, decimal, false);
    }
}

void runVoyager()
{
    // Матрица из общего задания учебного пособия.
    Matrix base{{INF,7,12,25,10},{10,INF,9,5,11},{13,8,INF,6,4},
                {6,11,15,INF,15},{5,9,12,17,INF}};
    runTour("Textbook common matrix", base);
    // Варианты задают размер и число удаленных дуг; веса выбраны для опыта.
    Matrix first = base;
    first[0][1] = INF;
    first[2][4] = INF;
    runTour("Variant 1", first);
    Matrix third = base;
    third[1][3] = INF;
    runTour("Variant 3", third);
    Matrix fourth{{INF,7,12,25,10,8},{10,INF,9,5,11,6},{13,8,INF,6,4,9},
                  {6,11,15,INF,15,7},{5,9,12,17,INF,10},{8,6,9,7,10,INF}};
    fourth[0][1] = INF;
    fourth[2][4] = INF;
    fourth[5][3] = INF;
    runTour("Variant 4", fourth);
}

int main()
{
    try
    {
        std::cout << "Laboratory work 4\n";
        int choice = -1;
        while (true)
        {
            std::cout << "\n1 - Voyager\n2 - Turing machine\n3 - All experiments\n0 - Exit\nChoice: ";
            if (!(std::cin >> choice) || choice == 0)
            {
                break;
            }
            if (choice == 1 || choice == 3)
            {
                runVoyager();
            }
            if (choice == 2 || choice == 3)
            {
                runTuring();
            }
            if (choice < 0 || choice > 3)
            {
                std::cout << "Invalid choice\n";
            }
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}