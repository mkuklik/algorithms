//
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
#include <map>

using namespace std;

template<typename T>
class DisjointSetForest {

    
    /*
     *  Element of a set
     */

    struct Node {
        T value;
        
        Node *parent{this};
        int rank {0};
        
        Node(T v): value(v) {};
    };
    
    vector<Node *> nodes;
    
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

public:
    
    /*
     *  make_set, create a new set and returns element's id
     */
    
    int make_set(T v) {
        
        nodes.push_back(new Node(v));
        
        return nodes.size()-1;
    }
    
    /*
     *  join is a union of two sets identified by two elements
     */
    
    void join(int a, int b) {
        
        link(find_set(nodes[a]), find_set(nodes[b]));
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
        
        for (int i=0; i<nodes.size(); i++)
            cout << nodes[i]->value << " " << m[find_set(nodes[i])] << endl;
        cout << endl;
    }
};


int main(int argc, const char * argv[]) {
    
    const int N = 10;
    
    DisjointSetForest<int> &d = *(new DisjointSetForest<int>);
    
    for (int i=0; i<N; i++)
        d.make_set(i);
    
    d.print();
    
    for (int i=1; i<N; i++) {
        
        d.join(0, i);
        d.print();
        cout << endl;
        
    }
    
    delete &d;
    
    cout << "done" << endl;
}
