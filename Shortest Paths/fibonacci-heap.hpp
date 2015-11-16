//
//  FibonacciHeap.hpp
//  workspace
//
//  Created by mkuklik on 11/15/15.
//  Copyright © 2015 mkuklik. All rights reserved.
//

#ifndef FibonacciHeap_hpp
#define FibonacciHeap_hpp

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

template<typename T>
struct FibonacciHeap {
    
    struct Node {
        /* each node can have multiple children at any level of the tree,
         * but it has one pointer to only one child. Each node has pointer
         * to parent. Each node that pointers to two sieblings, next and previous.
         */
        int key;
        T value;
        
        bool mark{false};
        /* The boolean-valued attribute x:mark indicates whether node x has
         * lost a child since the last time x was made the child of another node.
         */
        
        Node * child{nullptr};
        int degree {0}; // rank, number of children in the childrent linked-list
        
        Node * left{this};
        Node * right{this};
        
        Node * parent{nullptr};
        
        Node(int k, T v): key(k), value(v) {};
    };
    
    int N{0};                   // number of nodes in the heap
    Node * root {nullptr};       // points to the root list with lowest key
    
    /*
     *  Methods
     */
    
    ~FibonacciHeap();
    
    void deallocate(Node * n);
    
    void list_insert(Node * l, Node * r, Node * x);
    
    Node * heap_link(Node * y, Node * x);   // links two trees from the root linked list
    
    void consolidate();
    
    void cut(Node * x, Node * y);
    
    void cascading_cut(Node * y);
    
    Node * insert(int k, T v);       // insert key into the heap
    
    int get_min_key();              // get the lowest key
    
    Node * get_min_node();      // return pointer to min node, can be used to recover value

    void remove_min();          // remove the lowest key
    
    void decrease_key(Node * x, int k);
    
    void change_key(Node * x, int k);
    
    void remove(Node * x);      // remove Node
    
    int size() { return N; };
    
    /* priting/debugging functions */
    
    void print_node(Node * n, int ntab, int level, bool print_children=false);
    
    void print(int level);
};


//#include "FibonacciHeap.tpp"

#endif /* FibonacciHeap_hpp */
