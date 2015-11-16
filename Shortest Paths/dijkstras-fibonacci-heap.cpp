/*
 
 Dijkstra’s algorithm
 
 Implementation using binary heap from std library,
 in efficient as decreasing key can't be done without
 reheapifying the heap;
 
 --------------------------------------------------------------------
 
 Dijkstra’s algorithm
 
 Dijkstra’s algorithm solves the single-source shortest-paths problem
 on a weighted, directed graph G = (V, E) for the case in which all
 edge weights are nonnegative.
 
 Greedy strategies do not always yield optimal results in general,
 but it has been proven that Dijkstra’s algorithm does indeed compute shortest paths.
 
 Pseudocode:
 
 DIJKSTRA(G; w; s)
 
 1 INITIALIZE-SINGLE-SOURCE(G;s);
 3 Q  = G.V
 4 while !Q.isEmpty()
 5    u = EXTRACT-MIN(Q)
 6    S = S + {u}
 7    for each vertex v in G.adj[u]
 8          RELAX(u, v, w)
 
 The process of relaxing an edge (u,v) consists of testing whether we can improve the shortest path to v found so far by going through u and, if so, updating  v.d and v.prev . A relaxation step may decrease the value of the shortest-path estimate  v.d and update  v’s predecessor attribute v.prev. The following code performs a relaxation step on edge (u,v) in O(1) time:
 
 RELAX(u; v; w)
 1 if v.d > u.d + w(u,v)
 2      v.d = u.d + w(u,v)
 3      v.prev = u
 
 
 
 
 INITIALIZE-SINGLE-SOURCE(G; s)
 1 for each vertex v in G.V
 2     v.d = inf
 3     v.prev = NILL
 4  s.d = 0
 
 */

//  Created by mkuklik on 11/11/15.
//  Copyright © 2015 mkuklik. All rights reserved.


#include <iostream>
#include <vector>
#include <cmath>
#include <climits>

#include "fibonacci-heap.hpp"

using namespace std;

struct Graph {
    
    struct Edge {
        
        int to;
        int value;
        Edge * next{nullptr};
        
        Edge(int t, int v, Edge * n=nullptr): to(t), value(v), next(n) {};
    };
    
    int n_v {0};
    
    vector<Edge *> vertex;
    vector<Edge *> edges;
    
    // methods
    
    Graph(int n): n_v(n) {
        // preallocate memory for edges
        vertex = vector<Edge *> (n, nullptr);
        edges.reserve(100);
    }
    
    ~Graph() {
        for (auto e : edges)
            delete e;
    }
    
    const int n_vertices() const { return (int) n_v; };
    
    /*
     *  add edge
     */
    
    void add(int start, int end, int v) {
        
        if (vertex[start] == nullptr) {
            
            vertex[start] = new Edge(end, v);
            edges.push_back(vertex[start]);
            
        } else {
            // find last edge originating from 'start'
            Edge * last = vertex[start];
            while (last->next != nullptr )
                last = last->next;
            
            last->next = new Edge(end, v);
            edges.push_back(last->next);
        }
    }
    
    /*
     *  print
     */
    
    void print() {
        for (int i=0; i < n_v; i++) {
            cout << i << ": ";
            
            Edge * e = vertex[i];
            while (e != nullptr) {
                cout << e->to << "(" << e->value << ") ";
                e = e->next;
            }
            cout << endl;
        }
    }
};



/*
 *  Dijkstra's algorithms using Fibonacci Heap
 */


void dijkstras(const Graph &g, int s) {
    
    int n_v = g.n_vertices();
    
    FibonacciHeap<int> heap;    // heap node stores the vertex id
    
    typedef FibonacciHeap<int>::Node Node;
    
    vector<Node *> lookup(n_v, nullptr);
    vector<bool> visited(n_v);
    vector<int> dist(n_v);
    vector<int> previous(n_v);
    
    for (int i=0; i < n_v; i++) {
        dist[i] = (i == s) ? 0 : INT_MAX;
        lookup[i] = heap.insert(dist[i], i);   // lookup Node address, used to decrease a key
        previous[i] = -1;
    }
    
    
//    make_heap(vd.begin(), vd.end());   // version 1 & 2
    // or make_heap(vd.begin(), vd.end(), Vcomp); // version 3
    
    while (heap.size() > 0) {
        
        int v = heap.get_min_node()->value;  // vertex id with minimum key, i.e. distance
        
        heap.remove_min();
        
        visited[v] = true;  // v is removed from heap, keeps track of
                            // vertices on one side of the cut
        
        // Iterate over edges originating from v
        // and update distance to end vertices
        
        auto e = g.vertex[v];   // first edge
        while (e != nullptr) {
            
            int to {e->to}; // end vertex of the edge
            
            if (!visited[to] && dist[to] > dist[v] + e->value ) {
                
                dist[to] = dist[v] + e->value;
                previous[to] = v;
                
                // update heap
                heap.decrease_key(lookup[to], dist[to]);
            }
            e = e->next;
        }
    }
    
    
    // print shortest paths
    
    for (int i=0; i < n_v; i++)
        if (i != s) {
            cout << i << ", " << dist[i] << ": ";
            int p = previous[i];
            while (p != -1) {
                cout << p << ", ";
                p = previous[p];
            }
            cout << endl;
        }
}


int main(int argc, const char * argv[]) {
    
    //    Graph g(5);
    //    g.add(0, 1, 6);
    //    g.add(0, 4, 7);
    //    g.add(1, 2, 5);
    //    g.add(1, 3, 3);
    //    g.add(3, 2, 7);
    //    g.add(4, 3, 9);
    
    Graph g(9);
    g.add(0,1,4);
    g.add(0,7,8);
    g.add(1,2,8);
    g.add(2,3,7);
    g.add(2,5,4);
    g.add(2,8,2);
    g.add(7,8,7);
    g.add(7,1,11);
    g.add(7,6,1);
    g.add(6,5,2);
    g.add(6,8,6);
    g.add(5,3,14);
    g.add(5,4,10);
    g.add(3,4,9);
    
    g.print();
    
    dijkstras(g, 0);
    
    return 0;
}


/* Output
 
 
 0: 1(4) 7(8)
 1: 2(8)
 2: 3(7) 5(4) 8(2)
 3: 4(9)
 4:
 5: 3(14) 4(10)
 6: 5(2) 8(6)
 7: 8(7) 1(11) 6(1)
 8:
 1, 4: 0,
 2, 12: 1, 0,
 3, 19: 2, 1, 0,
 4, 21: 5, 6, 7, 0,
 5, 11: 6, 7, 0,
 6, 9: 7, 0,
 7, 8: 0,
 8, 14: 2, 1, 0,
 
 
 */

