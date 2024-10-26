#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <set>
#include <queue>
#include <limits>
#include <fstream>
using namespace std;

const int INF = numeric_limits<int>::max();
vector<int> dist;

struct Edge{
    int src, dest, weight;
    Edge(int src, int dest, int weight): src(src), dest(dest), weight(weight){}
};

struct Compare{
    bool operator()(int a, int b) const {
        return dist[a] > dist[b];
    }
};

class Graph{
public:
    int V;
    vector<int> Indeg;
    vector<vector<Edge>> adjList;
    vector<int> topo;
    Graph(int V, const vector<vector<Edge>>& graph): V(V){
        adjList = graph;
        vector<Edge> empty_edges;
        empty_edges.clear();
        adjList.push_back(empty_edges);
        Indeg.resize(V+1, 0);
        dist.resize(V+1, -1);
    }

    void Dijkstra(int src){
        dist[src] = 0;
        vector<int> parent(V+1);
        priority_queue<int, vector<int>, Compare> pq;
        pq.push(src);

        while(!pq.empty()){
            int curNode = pq.top();
            pq.pop();
            for (const auto& neighbor : adjList[curNode]){
                if (dist[curNode] != -1 && dist[curNode] + neighbor.weight > dist[neighbor.dest]){
                    dist[neighbor.dest] = dist[curNode] + neighbor.weight;
                    parent[neighbor.dest] = curNode;
                    pq.push(neighbor.dest);
                }
            }
        }
    }

    void Topo_sort(){
        queue<int> Q;
        for (int i = 1; i <= V; ++i){
            if (Indeg[i] == 0){
                Q.push(i);
            }
        }
        while(!Q.empty()){
            int current = Q.front();
            Q.pop();
            topo.push_back(current);
            for (const auto& neighbor : adjList[current]){
                Indeg[neighbor.dest]--;
                if (Indeg[neighbor.dest] == 0){
                    Q.push(neighbor.dest);
                }
            }
        }
    }
};

vector<vector<int>> find_position(const vector<int>& positive, const vector<int>& negative){
    vector<vector<int>> relative_right;
    if (positive.size() != negative.size()){
        cerr << "Error input!" << endl;
        return relative_right;
    }
    relative_right.resize(positive.size() + 1);
    unordered_map<int, int> index_map1, index_map2;
    for (int i = 0; i < positive.size(); ++i){
        index_map1[positive[i]] = i;
    }
    for (int i = 0; i < negative.size(); ++i){
        index_map2[negative[i]] = i;
    }
    for (int i = 0; i < positive.size(); ++i){
        for (int j = i + 1; j < positive.size(); ++j){
            if (index_map2[positive[j]] > index_map2[positive[i]]){
                relative_right[positive[i]].push_back(positive[j]);
            }
        }
    }
    return relative_right;
}

vector<vector<Edge>> Construct_Graph(const vector<vector<int>>& vec1, const vector<vector<int>>& vec2, const vector<int>& module_size){
    vector<vector<Edge>> constraint_graph;
    if (vec1.size() != vec2.size()){
        cerr << "Error input!" << endl;
        return constraint_graph;
    }

    constraint_graph.resize(2 * vec1.size() - 1);
    for (int i = 1; i < vec1.size(); ++i){
        if (vec1[i].size() == 0){
            constraint_graph[0].push_back(Edge(0, 2 * i - 1, 0));
        }
    }

    for (int i = 1; i < vec1.size(); ++i){
        if (vec2[i].size() == 0){
            constraint_graph[2 * i].push_back(Edge(2 * i, 2 * vec1.size() - 1, 0));
        }
    }

    for (int i = 1; i < vec1.size(); ++i){
        for (int j = 0; j < vec1[i].size(); ++j){
            constraint_graph[2 * vec1[i][j]].push_back(Edge(2 * vec1[i][j], 2 * i - 1, 0));
        }
    }

    for (int i = 1; i < vec1.size(); ++i){
        constraint_graph[2 * i - 1].push_back(Edge(2 * i - 1, 2 * i, module_size[i-1]));
    }
    return constraint_graph;
}

int main(){
    int module;
    cin >> module;
    string positive_sequence, negative_sequence;
    vector<int> positive_sequence_modules;
    vector<int> negative_sequence_modules;
    positive_sequence_modules.clear();
    negative_sequence_modules.clear();
    cin >> positive_sequence;
    for (char c : positive_sequence){
        positive_sequence_modules.push_back(int(c-'0'));
    }
    cin >> negative_sequence;
    for (char c : negative_sequence){
        negative_sequence_modules.push_back(int(c-'0'));
    }
    vector<pair<int, int>> module_size;
    for (int i = 0; i < module; ++i){
        string size;
        cin >> size;
        vector<int> sizes;
        sizes.clear();
        for (char c : size){
            sizes.push_back(int(c - '0'));
        }
        pair<int, int> size_pair = make_pair(sizes[0], sizes[1]);
        module_size.push_back(size_pair);
    }

    vector<int> horizontal_size;
    vector<int> vertical_size;
    for (int i = 0; i < module_size.size(); ++i){
        horizontal_size.push_back(module_size[i].first);
        vertical_size.push_back(module_size[i].second);
    }


    vector<int> positive_reverse_sequence, negative_reverse_sequence;
    positive_reverse_sequence = positive_sequence_modules;
    negative_reverse_sequence = negative_sequence_modules;
    reverse(positive_reverse_sequence.begin(), positive_reverse_sequence.end());
    reverse(negative_reverse_sequence.begin(), negative_reverse_sequence.end());
    vector<vector<int>> relative_right;
    vector<vector<int>> relative_left;
    vector<vector<int>> relative_above;
    vector<vector<int>> relative_below;
    relative_right = find_position(positive_sequence_modules, negative_sequence_modules);
    relative_left = find_position(positive_reverse_sequence, negative_reverse_sequence);
    relative_above = find_position(positive_reverse_sequence, negative_sequence_modules);
    relative_below = find_position(positive_sequence_modules, negative_reverse_sequence);
    
    vector<vector<Edge>> horizontal_graph;
    vector<vector<Edge>> vertical_graph;
    horizontal_graph = Construct_Graph(relative_left, relative_right, horizontal_size);
    vertical_graph = Construct_Graph(relative_below, relative_above, vertical_size);
    Graph graph1(2 * module + 2, horizontal_graph);
    graph1.Dijkstra(0);
    vector<int> lower_left_first;
    vector<int> lower_right_second;
    for (int i = 1; i <= module; ++i){
        lower_left_first.push_back(dist[2 * i - 1]);
    }
    cout << "Width of the layout: ";
    cout << dist[2 * module + 1] << endl;
    dist.clear();
    Graph graph2(2 * module + 2, vertical_graph);
    graph2.Dijkstra(0);
    for (int i = 1; i <= module; ++i){
        lower_right_second.push_back(dist[2 * i - 1]);
    }
    cout << "Height of the layout: ";
    cout << dist[2 * module + 1] << endl;
    cout << "Lower-left corner of each module: " << endl;
    dist.clear();
    vector<pair<int, int>> lower_left_corner;
    for (int i = 0; i < lower_left_first.size(); ++i){
        lower_left_corner.push_back(make_pair(lower_left_first[i], lower_right_second[i]));
    }
    for (int i = 0; i < lower_left_corner.size(); ++i){
        cout << "Module " << i+1 << ": ";
        cout << "(" << lower_left_corner[i].first << "," << lower_left_corner[i].second << ")" << endl;
    }
    return 0;
}