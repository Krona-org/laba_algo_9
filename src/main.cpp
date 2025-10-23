#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

class Graph
{
private:
    int vertexCount;
    vector<vector<int>> matrix;
    vector<vector<int>> adjList;
    vector<bool> visited;

public:
    Graph(int n) : vertexCount(n)
    {
        matrix.assign(n, vector<int>(n, 0));
        adjList.assign(n, {});
        visited.assign(n, false);
    }

    // 1.1 Генерация матрицы смежности
    void genAdjMatrix(double probability)
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);

        // создаём цепочку для связности
        for (int i = 0; i < vertexCount - 1; ++i)
        {
            matrix[i][i + 1] = matrix[i + 1][i] = 1;
        }

        // добавляем случайные рёбра
        for (int i = 0; i < vertexCount; ++i)
        {
            for (int j = i + 1; j < vertexCount; ++j)
            {
                if (dis(gen) < probability)
                {
                    matrix[i][j] = 1;
                }
            }
        }

        // создаём список смежности
        for (int i = 0; i < vertexCount; ++i)
        {
            adjList[i].clear();
            for (int j = 0; j < vertexCount; ++j)
            {
                if (matrix[i][j])
                    adjList[i].push_back(j);
            }
        }

        cout << "Матрица смежности создана.\n";
    }

    // 1.2 BFS (матрица смежности)
    void BFS(int start)
    {
        vector<int> dist(vertexCount, -1);
        queue<int> q;
        dist[start] = 0;
        q.push(start);

        while (!q.empty())
        {
            int v = q.front();
            q.pop();

            for (int i = 0; i < vertexCount; ++i)
            {
                if (matrix[v][i] == 1 && dist[i] == -1)
                {
                    dist[i] = dist[v] + 1;
                    q.push(i);
                }
            }
        }

        cout << "BFS (матрица смежности):\n";
        for (int i = 0; i < vertexCount; ++i)
            cout << "DIST[" << i << "] = " << dist[i] << "\n";
    }

    // 1.3 BFS (список смежности)
    void BFS_adj(int start)
    {
        vector<int> dist(vertexCount, -1);
        queue<int> q;
        dist[start] = 0;
        q.push(start);

        while (!q.empty())
        {
            int v = q.front();
            q.pop();

            for (int nb : adjList[v])
            {
                if (dist[nb] == -1)
                {
                    dist[nb] = dist[v] + 1;
                    q.push(nb);
                }
            }
        }

        cout << "BFS (список смежности):\n";
        for (int i = 0; i < vertexCount; ++i)
            cout << "DIST[" << i << "] = " << dist[i] << "\n";
    }

    // 2. DFS (матрица смежности)
    void DFS_matrix(int v, int depth = 0, vector<int>* distPtr = nullptr)
    {
        visited[v] = true;
        if (distPtr) (*distPtr)[v] = depth;

        for (int i = 0; i < vertexCount; ++i)
        {
            if (matrix[v][i] == 1 && !visited[i])
                DFS_matrix(i, depth + 1, distPtr);
        }
    }

    // 2. DFS (список смежности)
    void DFS_list(int v, int depth = 0, vector<int>* distPtr = nullptr)
    {
        visited[v] = true;
        if (distPtr) (*distPtr)[v] = depth;

        for (int nb : adjList[v])
        {
            if (!visited[nb])
                DFS_list(nb, depth + 1, distPtr);
        }
    }

    void resetVisited()
    {
        fill(visited.begin(), visited.end(), false);
    }

    // 3. Замер времени (как было)
    void TimeToTime(void (Graph::* func)(int), int start)
    {
        auto t1 = high_resolution_clock::now();
        (this->*func)(start);
        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<microseconds>(t2 - t1).count();
        cout << "\nВремя выполнения: " << dur << " мкс\n\n";
    }

    // 3.2 Замер DFS
    void TimeToTimeDFS(void (Graph::* func)(int, int, vector<int>*), int start)
    {
        vector<int> dist(vertexCount, -1);
        resetVisited();

        auto t1 = high_resolution_clock::now();
        (this->*func)(start, 0, &dist);
        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<microseconds>(t2 - t1).count();

        cout << "DFS результаты:\n";
        for (int i = 0; i < vertexCount; ++i)
            cout << "DIST[" << i << "] = " << dist[i] << "\n";

        cout << "\nВремя выполнения: " << dur << " мкс\n\n";
    }
    void BFS_shortest_path(int start, int end)
    {
        vector<int> dist(vertexCount, -1);  // расстояния
        vector<int> parent(vertexCount, -1); // родитель вершины для восстановления пути
        queue<int> q;

        dist[start] = 0;
        q.push(start);

        while (!q.empty())
        {
            int v = q.front();
            q.pop();

            for (int i = 0; i < vertexCount; ++i)
            {
                if (matrix[v][i] == 1 && dist[i] == -1)
                {
                    dist[i] = dist[v] + 1;
                    parent[i] = v;  // запоминаем, откуда пришли
                    q.push(i);
                }
            }
        }

        // Восстанавливаем путь
        if (dist[end] == -1)
        {
            cout << "Путь от " << start << " до " << end << " не существует.\n";
            return;
        }

        vector<int> path;
        for (int v = end; v != -1; v = parent[v])
            path.push_back(v);
        reverse(path.begin(), path.end());

        cout << "Кратчайший путь от " << start << " до " << end << ": ";
        for (int v : path)
            cout << v << " ";
        cout << "\nДлина пути: " << dist[end] << "\n";
    }
};
int main()
{
    setlocale(LC_ALL, "RUSSIAN");

    int n;
    double p;

    cout << "Введите количество вершин графа: ";
    cin >> n;
    cout << "Введите вероятность ребра (0.1 - 0.9): ";
    //cin >> p;
    p = 0.3;

    Graph g(n);
    g.genAdjMatrix(p);

    cout << "\n--- Обход в ширину (матрица смежности) ---\n";
    g.TimeToTime(&Graph::BFS, 0);

    cout << "\n--- Обход в ширину (список смежности) ---\n";
    g.TimeToTime(&Graph::BFS_adj, 0);

    cout << "\n--- Обход в глубину (матрица смежности) ---\n";
    g.TimeToTimeDFS(&Graph::DFS_matrix, 0);

    cout << "\n--- Обход в глубину (список смежности) ---\n";
    g.TimeToTimeDFS(&Graph::DFS_list, 0);

    cout << '\n';
    g.BFS_shortest_path(0, 4);
    return 0;
}
