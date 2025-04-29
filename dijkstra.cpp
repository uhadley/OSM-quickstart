#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <limits>

using namespace std;

typedef pair<string, double> label;
unordered_map<long long, unordered_map<long long, label>> graph;

// Heap node
struct Node {
    long long id;
    double dist;
};

vector<Node> heap;
unordered_map<long long, int> heapIndex;

int parent(int i) { return (i - 1) / 2; }
int left(int i) { return 2 * i + 1; }
int right(int i) { return 2 * i + 2; }

void swapNodes(int i, int j) {
    swap(heap[i], heap[j]);
    heapIndex[heap[i].id] = i;
    heapIndex[heap[j].id] = j;
}

void heapifyUp(int i) {
    while (i > 0 && heap[parent(i)].dist > heap[i].dist) {
        swapNodes(i, parent(i));
        i = parent(i);
    }
}

void heapifyDown(int i) {
    int smallest = i;
    int l = left(i);
    int r = right(i);

    if (l < heap.size() && heap[l].dist < heap[smallest].dist)
        smallest = l;
    if (r < heap.size() && heap[r].dist < heap[smallest].dist)
        smallest = r;

    if (smallest != i) {
        swapNodes(i, smallest);
        heapifyDown(smallest);
    }
}

void pushHeap(Node n) {
    heap.push_back(n);
    heapIndex[n.id] = heap.size() - 1;
    heapifyUp(heap.size() - 1);
}

Node popHeap() {
    Node top = heap[0];
    heapIndex.erase(top.id);

    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty()) {
        heapIndex[heap[0].id] = 0;
        heapifyDown(0);
    }

    return top;
}

void decreaseKey(long long id, double newDist) {
    int i = heapIndex[id];
    if (heap[i].dist > newDist) {
        heap[i].dist = newDist;
        heapifyUp(i);
    }
}

bool inHeap(long long id) {
    return heapIndex.find(id) != heapIndex.end();
}

void printDistances(const unordered_map<long long, double>& distance) {
    cout << "Current distances:\n";
    for (const auto& [node, dist] : distance) {
        cout << "  Node " << node << ": ";
        if (dist == numeric_limits<double>::infinity())
            cout << "INF";
        else
            cout << dist;
        cout << endl;
    }
    cout << "----------------------------\n";
}

void dijkstra(long long start) {
    unordered_map<long long, double> distance;
    unordered_map<long long, long long> previous;
    unordered_map<long long, bool> visited;

    for (auto& node : graph){
        distance[node.first] = numeric_limits<double>::infinity();
        for (auto n:node.second)
            distance[n.first]= numeric_limits<double>::infinity();
        
    }
    distance[start]=0;
    

    pushHeap({start, 0.0});

    while (!heap.empty()) {
        Node current = popHeap();
        long long u = current.id;

        if (visited[u]) continue;
        visited[u] = true;

        cout << "Visiting node " << u << " (distance = " << current.dist << ")\n";
        printDistances(distance);

        for (auto& neighbor : graph[u]) {
            long long v = neighbor.first;
            double weight = neighbor.second.second;

            if (distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                previous[v] = u;

                if (inHeap(v)) {
                    decreaseKey(v, distance[v]);
                } else {
                    pushHeap({v, distance[v]});
                }
            }
        }
    }

    cout << "\nFinal shortest distances from node " << start << ":\n";
    for (const auto& [node, dist] : distance) {
        cout << "Node " << node << ": ";
        if (dist == numeric_limits<double>::infinity()) cout << "unreachable";
        else cout << dist;
        cout << endl;
    }
}

int main() {
    graph[1][2] = {"road", 4.5};
    graph[1][3] = {"bridge", 2.0};
    graph[2][3] = {"tunnel", 1.0};
    graph[2][4] = {"highway", 5.0};
    graph[3][4] = {"street", 8.0};
    graph[3][5] = {"alley", 10.0};
    graph[4][5] = {"path", 2.0};

    dijkstra(1);

    return 0;
}
