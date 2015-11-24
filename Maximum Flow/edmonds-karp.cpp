//
//  main.cpp
//  Ford-Fulkerson
//
//  Created by mkuklik on 11/15/15.
//  Copyright © 2015 mkuklik. All rights reserved.
//

#include <iostream>
#include <vector>
#include <iterator>
#include <queue>

/* 
 *  Graph is a data structure that stores original and residual graph
 *  in one graph. When original edge is added to Graph, two edges
 *  are created, original and reverse, where the latter is indicated
 *  by is_reverse == true. Corresponding edges are linked via a pointer,
 *  "other". In original edge residual_capacity is equal to (_capacity - _flow)
 *  in a reverse edge residual capacity is equal to _flow in the origianal edge.
 *  Updating flow in original or residual edges changes appropriate variables in
 *  both edges. update_*() is edge-type conscious
 *  
 *  class Edge is public but its content is public-constant or private, accessible only
 *  via api: capacity, flow, residual_capacity, update_to, update_by
 *  
 *  Access to the edges in original graph is via an iterator, "iterator", while access to the edges
 *  in residual graph is via "resid_iterator".
 */

class Graph {
public:
    class Edge {
        friend Graph;
    public:
        const int from;
        const int to;
        const bool is_reverse {false};
    private:
        int _capacity;  // the same in both original and reverse egde
        int _flow {0};  // only used in original edge (is_reverse == false)
        int _residaul_capacity;
        
        Edge * other{nullptr}; // pointer to related edge
        
        Edge * next{nullptr};
        
        Edge(int f, int t, int c, bool ir=false): from(f), to(t), _capacity(c), is_reverse(ir),
        _residaul_capacity(c) {};
        
    public:
        
        /*
         *  Update flow to value f, note that function
         *      distingishes between original and reverse edges
         *      and update flows and residual_capacity in both edges
         *      respectively
         */
        
        void update_to(int f) {
            if (!is_reverse) {
                if (f < 0) throw "flow < 0";
                if (f > _capacity) throw "flow > capacity";
                _flow = f;
                _residaul_capacity = _capacity - f;
                other->_residaul_capacity = f;
            }
            else {
                if (f < 0) throw "flow < 0";
                if (f > other->_capacity) throw "flow > capacity";
                other->_flow = f;
                other->_residaul_capacity = other->_capacity - f;
                _residaul_capacity = f;
            }
        }
        
        void update_by(int f) {
            update_to(_flow + f);
        }
        
        int capacity() { return _capacity; }
        int flow() { return _flow; }
        int residaul_capacity() { return _residaul_capacity; }
        
    };
    
private:
    
    const int N {0};
    
    std::vector<Edge *> vertices;
    
    /*
     *  add edge to a adjacency list
     */
    
    void add_edge(int from, Edge *e) {
        if (vertices[from] == nullptr) {
            vertices[from] = e;
        } else {
            auto last = vertices[from];
            while (last->next != nullptr )
                last = last->next;
            
            last->next = e;
        }
    }
    
public:
    
    Graph(int n): N(n), vertices(std::vector<Edge *>(n)) {};
    
    ~Graph() {
        for (int i=0; i<N; i++) {
            Edge * e = vertices[i];
            while (e != nullptr) {
                auto tmp = e;
                e = e->next;
                delete tmp;
            }
        }
    }
    
    const int n_vertices() const { return N; };
    
    /*
     *  insert edge
     */
    
    void insert(const int from, const int to, const int capacity) {
        
        Edge * e = new Edge(from, to, capacity);
        Edge * rev = new Edge(to, from, capacity, true);
        
        // update original edge
        e->_flow = 0;
        e->other = rev;
        e->_residaul_capacity = capacity;
        
        add_edge(from, e);  // insert e

        // update reverse edge
        rev->_flow = 0;
        rev->other = e;
        rev->_residaul_capacity = 0;
        
        add_edge(to, rev);  // insert rev
    }
    
    /*
     *  original graph iterator
     */
     
    class iterator : std::iterator<std::forward_iterator_tag, const Edge> {
        Edge * p;
        
    public:
        iterator(Edge * e): p(e) {};
        iterator(const iterator & it): p(it.p) {};
        iterator & operator++() {
            // making sure that next edge is not a reverse edge
            if (p!=nullptr) {
                p = p->next;
                while (p != nullptr && p->is_reverse==true)
                    p = p->next;
            }
            return *this;
        };
        iterator operator++(int) { iterator tmp(*this); ++tmp; return tmp; };
        bool operator==(const iterator & x) { return p == x.p; }
        bool operator!=(const iterator & x) { return p != x.p; }
        Edge & operator*() { return *p; }
    };
    
    /*
     *      residual graph iterator
     */
    class resid_iterator : std::iterator<std::forward_iterator_tag, const Edge> {
        Edge * p;
        
    public:
        resid_iterator(Edge * e): p(e) {};
        resid_iterator(const resid_iterator & it): p(it.p) {};
        resid_iterator & operator++() {
            // making sure that edge has positive residual capacity
            if (p!=nullptr) {
                p = p->next;
                while (p != nullptr && p->_residaul_capacity==0)
                    p = p->next;
            }
            return *this;
        };
        resid_iterator operator++(int) { resid_iterator tmp(*this); ++tmp; return tmp; };
        bool operator==(const resid_iterator & x) { return p == x.p; }
        bool operator!=(const resid_iterator & x) { return p != x.p; }
        Edge & operator*() { return *p; }
    };
    
    /*
     *  access to the original graph iterator
     */
    
    iterator begin(int i) {
        auto p = vertices[i];
        while (p != nullptr && p->is_reverse)
            p = p->next;
        return iterator(p);
    }
    
    iterator end(int i=0) { return iterator(nullptr); }
    
    /*
     *  access to the residual graph iterator
     */
    
    resid_iterator resid_begin(int i) {
        auto p = vertices[i];
        while (p != nullptr && p->_residaul_capacity == 0.0)
            p = p->next;
        return resid_iterator(p);
    }
    
    resid_iterator resid_end(int i=0) { return resid_iterator(nullptr); }
    
    /*
     *   print
     */
    
    void print() {
        for (int i=0; i < N; i++) {
            std::cout << i << ": ";
            
            for (auto it=begin(i); it != end(); ++it)
                std::cout << (*it).to << "(" << (*it)._flow <<
                "/" << (*it)._capacity << ") ";
            std::cout << std::endl;
        }
    }
    
    void resid_print() {
        for (int i=0; i < N; i++) {
            std::cout << i << ": ";
            
            for (auto it=resid_begin(i); it != resid_end(); ++it) {
                std::cout << (*it).to;
                if ((*it).is_reverse) std::cout << "r";
                std::cout << "(" << (*it)._flow <<
                ", " << (*it)._capacity << ") ";
            }
            std::cout << std::endl;
        }
    }
    
};


/*
 *  Ford - Fulkerson algorithms
 *  Edmonds Karp version with augmented path search using
 *      BFS with unit edges
 */

int edmonds_karp(Graph &g, int start, int end)
{
    int N = g.n_vertices();
    
    // reset all flows to zero
    for (int i=0; i<N; i++) {
        for (auto it=g.begin(i); it != g.end(); ++it) {
            (*it).update_to(0.0);
        }
    }
    
    // while there exists a path p from s to t in the residual graph;
    while (true) {

        // find shortest path in residual graph using BFS under assumption
        // that each edge has unit distance.
        
        // BFS BEGIN
        
        enum COLOR { WHITE, GRAY, BLACK};
        std::vector<COLOR> color(N, WHITE);
        std::vector<int> dist(N, 0);
        std::vector<int> previous(N, -1);
        std::vector<Graph::resid_iterator> edge_to(N, nullptr); // store pointers to edges
                                                                // leading to vertex v
        std::queue<int> q;
        q.push(start);
        color[start] = GRAY;
        dist[start] = 0;    // redundant, only here for exposition of BFS
        
        while (q.size() > 0) {
            
            int v = q.front();
            q.pop();
            
            for (auto it=g.resid_begin(v); it != g.resid_end(); ++it) {
                int to = (*it).to;
                if (color[to] == WHITE) {
                    q.push(to);
                    color[to] = GRAY;
                    
                    dist[to] = dist[v] + 1;
                    previous[to] = v;
                    edge_to[to] = it;   // save edge leading to vertex "to"
                }
            }
            color[v] = BLACK;
        }
        // BFS END
        
        // if (no path) break
        if (previous[end] == -1) break;
        
        // finding minimum residual_capacity along the path
        int cf {INT_MAX};
        int i {end};
        while (i != start) {
            int tmp = (*edge_to[i]).residaul_capacity();
            if (tmp < cf) cf = tmp;
            i = previous[i];
        }
        
        // update graph
        i = end;
        while (i != start) {
            (*edge_to[i]).update_by(cf);    // notice that update() distinguishes
            i = previous[i];                // between original and reverse edge
        }
    }
    
    // calculate maximum from the start node
    int max_flow {0};
    for (auto it=g.begin(start); it != g.end(0); ++it)
        max_flow += (*it).flow();
    
    return max_flow;
}



/* 
 *  main
 */

int main(int argc, const char * argv[]) {
    
    // Example 1
    // CLRS 2009 p 726
    
    /*           v1 -> 12 -> v3
     /|         |/    \|
     16  |\    9     |\    20
     s=v0      4  |/        7       t=v5
     13  |           |  /|  4
     v2 -> 14 -> v4
     
     */
    
    
    Graph g(6);
    
    g.insert(0,1,16);
    g.insert(0,2,13);
    g.insert(1,3,12);
    g.insert(2,4,14);
    g.insert(2,1,4);
    g.insert(3,2,9);
    g.insert(3,5,20);
    g.insert(4,5,4);
    g.insert(4,3,7);
    
    std::cout << "\n Example 1\n\n";
    g.print();
    
    auto m1 = edmonds_karp(g, 0, 5);
    
    std::cout << "\nmax flow is " << m1 << std::endl << std::endl <<
                 "flow graph: \n";
    g.print();
    
    // Example 2
    // CLRS 2009 p 728
    Graph g2(4);
    g2.insert(0, 1, 1000000);
    g2.insert(0, 3, 1000000);
    g2.insert(1, 2, 1000000);
    g2.insert(1, 3, 1);
    g2.insert(3, 2, 1000000);
    
    std::cout << " ------------------ \n";
    std::cout << "\n Example 2\n\n";
    g2.print();
    
    auto m2 = edmonds_karp(g2, 0, 2);
    
    std::cout << "\nmax flow is " << m2 << std::endl << std::endl <<
                  "flow graph: \n";

    g2.print();
    
    // Example 3
    // online
    
    Graph g3(4);
    g3.insert(0, 1, 3);
    g3.insert(0, 3, 2);
    g3.insert(1, 2, 2);
    g3.insert(1, 3, 3);
    g3.insert(3, 2, 4);
    
    std::cout << " ------------------ \n";
    std::cout << "\n Example 3 \n\n";
    g3.print();
    
    auto m3 = edmonds_karp(g3, 0, 2);
    
    std::cout << "\nmax flow is " << m3 << std::endl << std::endl <<
                 "flow graph: \n";
    g3.print();
    
    return 0;
}
