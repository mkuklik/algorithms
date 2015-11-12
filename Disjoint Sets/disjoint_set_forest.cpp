///
//  Disjoint Set Forest
//
//  implementation of discoint set using trees instead of linked-list
//  * much cleaner and allows for each access to each node
//
//  Created by mkuklik on 11/8/15.
//  Copyright © 2015 mkuklik. All rights reserved.
//

#include <iostream>
#include <vector>
#include <map>      // only for printing

using namespace std;

class DisjointSetForest {
    
    
    /*
     *  Element of a set
     */
    
    struct Node {
        
        Node *parent{this}; // self-reference at initiation
        int rank {0};
        
        Node() {};
    };
    
    
    int N;                  // original number of sets
    int nsets;              // current number of distinguished sets
    vector<Node *> nodes;   // node lookup
    
    /*
     *  Link two trees
     */
    
    void link(Node *x, Node *y) {
        
        if (x->rank > y->rank)
            
            y->parent = x;
        
        else {
            
            x->parent = y;
            
            if (x->rank == y->rank) // if both are equal rank then linking x to y
                // will increase y's rank
                y->rank++;
        }
    }
    
    /*
     *  find head node of the set, which contains element a;
     */
    
    Node * find_set(Node *x) {
        
        if (x->parent != x)
            x->parent = find_set(x->parent);    // path compression
        
        return x->parent;
    }
    
    //    // no path compression
    //
    //    Node * find_set(int a) {
    //
    //        Node *e {nodes[a]};
    //        while (e->parent != e)
    //            e = e->parent;
    //
    //        return e;
    //    }
    
    /*
     *  make_set, create a new set and returns element's id
     */
    
    int make_set() {
        
        nodes.push_back(new Node());
        
        return nodes.size()-1;
    }
    
public:
    
    DisjointSetForest(int n): N(n), nsets(N) {
        
        nodes.reserve(n);   // reallocating memory
        
        for (int i=0; i<N; i++)
            nodes.push_back(new Node());
    }
    
    ~DisjointSetForest() {
        
        // deallocate all the nodes
        for (auto n : nodes)
            delete n;
    }
    
    /*
     *  join is a union of two sets identified by two elements
     */
    
    void join(int a, int b) {
        
        Node *pa = find_set(nodes[a]);
        Node *pb = find_set(nodes[b]);
        
        
        if (pa != pb) {
            
            link(pa, pb);
            --nsets;
        }
    }
    
    
    /*
     *  n_sets, returns number of sets
     */
    
    int n_sets() { return nsets; }
    
    
    /*
     *  compares if two elements are from the same set
     */
    bool same_set(int a, int b) {
        return find_set(nodes[a]) == find_set(nodes[b]);
    }
    
    
    /*
     *  print sets
     */
    
    void print() {
        int n = nodes.size();
        
        map<Node *, int> m;
        
        int set_counter {0};
        
        for (int i=0; i<n; i++) {
            Node *s = find_set(nodes[i]);
            
            auto x = m.find(s);
            
            if (x ==  m.end())
                m.insert(std::pair<Node *, int>(s, set_counter++));
        }
        
        std::cout << nsets << ": ";
        for (int i=0; i < nodes.size(); i++)
            cout << i << " P(" << m[find_set(nodes[i])] << ") ";
        cout << endl;
    }
};


int main(int argc, const char * argv[]) {
    
    const int N = 10;
    
    DisjointSetForest &d = *(new DisjointSetForest(N));
    
    d.print();
    std::cout << std::endl;
    
    // pick two integers upto N randomely and join corresponding sets
    while (d.n_sets() > 1) {
        int i = rand() % N;
        int j = rand() % N;
        
        // print elements
        std::cout << i << "  " << j << "  ";
        
        if (d.same_set(i, j))
            
            std::cout << "same set\n";
        
        else {
            
            std::cout << "joining\n";
            
            d.join(i, j);
        }
        
        // print elements
        std::cout << "\n\t";
        d.print();
        std::cout << std::endl;
    }
    
    delete &d;
    
    cout << "done" << endl;
}
