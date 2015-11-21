/*
 
 The Bellman-Ford algorithm
 
 The Bellman-Ford algorithm solves the single-source shortest-paths 
 problem in the general case in which edge weights may be negative.
 
 BELLMAN-FORD(G; w; s)
 1  INITIALIZE-SINGLE-SOURCE(G; s) {
        - set distance on all vertecies to inf
          and distance on s to 0;
        - reset
    }
 2  for i = 1 to |G.V| - 1
 3      for each edge(u,v) in G.E
 4           RELAX(u; v; w)
 5  for each edge (u,v) in G:E
 6       if  v.d > u.d + w(u,v)
 7           return FALSE
 8  return TRUE
 
 RELAX(u,v,w)
    if (dist(u) + w < dist(v)){
        dist(v) = dist(u) + w
        prev(v) = u
    }
 
 Negative cycle detection
 Proof by contradiction.
 Assume that there's a negative weigth cycle  <v0,..,vn> where v0=vn, and suppose Bellman-Ford
 returns true.
 By triangular inquality we have that
 v.d = sp(s,v) <= s(s,u) + w(u,v) = u.d + w(u,v)
 Sum over all the vertecies in the cycle
 sum(v(i-1).d, i, {1,n}) <= sum( s(s,v(i)), i, {1,n}) + sum(w(v(i-1), v(i), i, {1,n})
 Since v0=vn two first sums are equal so
 
 sum(w(v(i-1), v(i), i, {1,n}) >= 0
 
 But <v0,..,vn> is a negative cycle, i.e. sum(w(v(i-1), v(i), i, {1,n}) < 0
 
 BF must return false when there's a negative cycle
 
 Summary
 BFS: shortest path in an undirected graph with unit weigth edges.
 Bellman-Ford: shortest path in directed graph with positive and negative weight edges
                - detects whether negative cycle is reachable from the source.
 
 Dijkstras: shortest path in directed graph with non-negative weight edges, O(V+E)
 
 
 
 */


//  Created by mkuklik on 11/11/15.
//  Copyright © 2015 mkuklik. All rights reserved.
//


#include <iostream>
#include <vector>
#include <stack>

using namespace std;

/*
 *
 */

template<typename T>
struct Graph {
    
    struct Edge {
        T value;
        int from;
        int to;
        Edge* next{nullptr};
        
        Edge(int f, int t, int v, Edge* n=nullptr): from(f), to(t), value(v), next(n) {};
    };
    
    const int N; // number of vertecies
    
    vector<Edge *> vertex;  //link vertex to first edge in linked-list
    vector<Edge *> edges;
    
    Graph(int n): vertex(vector<Edge *>(n, nullptr)), N(n) {};
    ~Graph();
    
    int add(int f, int t, T value);
    const int nvertex() const { return N; }
    void print();
};

template<typename T>
Graph<T>::~Graph() {
    // deallocate all existing edges
    for (int i=0; i<N; i++) {
        Edge *e {vertex[i]};
        while (e != nullptr) {
            Edge *tmp {e->next};
            delete e;
            e = tmp;
        }
    }
}

/*
 *  Add edge
 */

template<typename T>
int Graph<T>::add(int f, int t, T v) {
    Edge *e = new Edge(f, t, v);
    
    if (vertex[f] == nullptr) {
        vertex[f] = e;
    } else {
        Edge *n {vertex[f]};
        
        // find last Edge in the linked list
        while (n->next != nullptr)
            n = n->next;
        n->next = e;
    }
    
    // get edge id and return it to client
    edges.push_back(e);
    return (int) edges.size() - 1;
}

/*
 *  Print graph
 */

template<typename T>
void Graph<T>::print() {
    cout << endl;
    
    for (int i=0; i<N; i++) {
        cout << i << ": ";
        
        Edge *e {vertex[i]};
        while (e != nullptr) {
            cout << e->to << " (" << e->value << ")  ";
            e = e->next;
        }
        cout << endl;
    }
}
    

/*
 *  BellmanFord single-source Shortest Path algorithm
 */


class BellmanFord {

    const int N;
    const Graph<int> &g;
    
    vector<int> dist;
    vector<int> prev;
    
    int start;
    bool noNegativeCycles {true};
    
public:
    BellmanFord(const Graph<int> &gg):  N(gg.nvertex()), g(gg), dist(vector<int>(N, 0)),
                                 prev(vector<int>(N, -1)) {};
    
    /*
     *  shortest Paths from s
     */
    
    bool shortestPathFrom(int s) {
        
        start = s;
        
        for (int i=0; i<N; i++) {
            
            dist[i] = INT_MAX;
            prev[i] = -1;
        }
        
        dist[s] = 0;
        prev[s] = s;
        
        for (int i=0; i<N-1; i++) {
            
            // loop over all edges
            for (auto e: g.edges) {    // or (Graph<int>::Edge *e: g.edges)
                if (dist[e->from] == INT_MAX) continue;
                
                // relax an edge
                if (dist[e->from] + e->value < dist[e->to]) {
                    
                    dist[e->to] = dist[e->from] + e->value;
                    prev[e->to] = e->from;
                }
            }
        }
        
        // detect cycles
        for (auto e: g.edges) {
            
            if (dist[e->from] + e->value < dist[e->to]) {
                noNegativeCycles = false;
                return false;
            }
        }
        return true;
    }
    
    /*
     *  Print results
     */
    
    void print() {
        if (!noNegativeCycles) {
    
            std::cout << "negative cycles detected\n";
        }
        else {
            cout << "starting from " << start << endl;

            for (int i=0; i<N; i++) {
                
                cout << i << " d(" << dist[i] << ")";
                int p {i};
    
                while (p != start) {
                    cout << " <- " << prev[p];
                    p = prev[p];
                }
                
                std::cout << std::endl;
            }
        }

    }
    
};





int main(int argc, const char * argv[]) {
    
    // Graph 1
    
    Graph<int> g(9);
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
    
    BellmanFord bfg(g);
    
    bfg.shortestPathFrom(0);

    cout << "\n1\n\n";
    cout << "Graph:\n";
    g.print();
    cout << "nShortest paths\n";
    bfg.print();
    
    
    // Graph 2
    // Fig 14.4 p 652 in CLRS
    
    Graph<int> h(5);
    h.add(0,1,6);
    h.add(0,4,7);
    h.add(1,2,5);
    h.add(2,1,-2);
    h.add(1,3,-4);
    h.add(1,4,8);
    h.add(4,3,9);
    h.add(4,2,-3);
    h.add(3,2,7);
    h.add(3,0,2);
    
    BellmanFord bfh(h);
    
    bfh.shortestPathFrom(0);
    
    cout << "\n2\n\n";
    cout << "Graph:\n";
    h.print();
    cout << "\nnShortest paths\n\n";
    bfh.print();
    
    
    // Graph 3
    // Fig 14.4 from CLRS modified for negative cycle
    
    Graph<int> hn(5);
    hn.add(0,1,6);
    hn.add(0,4,7);
    hn.add(1,2,5);
    hn.add(2,1,-3); // try x < -2 ; original -2
    hn.add(1,3,-4);
    hn.add(1,4,8);
    hn.add(4,3,9);
    hn.add(4,2,-3);
    hn.add(3,2,7); //try 5, original 7
    hn.add(3,0,2);
    
    BellmanFord bfhn(hn);
    
    bfhn.shortestPathFrom(0);
    
    cout << "\n3\n\n";
    cout << "Graph:\n";
    hn.print();
    cout << "\nnShortest paths\n\n";
    bfhn.print();

    return 0;
}




