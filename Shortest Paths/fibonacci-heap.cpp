//
//  FibonacciHeap.cpp
//  workspace
//
//  Created by mkuklik on 11/15/15.
//  Copyright © 2015 mkuklik. All rights reserved.
//

#include "fibonacci-heap.hpp"

/*
 *  insert a node in between note l (on the left) and r (on the right)
 *        in the linked list
 */

template<typename T>
void FibonacciHeap<T>::list_insert(Node * x, Node * l, Node * r) {
    // insert Node x between node l and r in double-linked list
    
    x->left = l;
    x->right = r;
    
    r->left = x;
    l->right = x;
}

/*
 *  insert new node with key k and value v
 */

template<typename T>
typename FibonacciHeap<T>::Node * FibonacciHeap<T>::insert(int k, T v) {
    
    Node * tmp = new Node(k, v);
    
    if (root == nullptr) {
        
        root = tmp;
    }
    else {
        
        list_insert(tmp, root->left, root);
        
        if (tmp->key < root->key)
            root = tmp;
    }
    ++N;
    
    return tmp;
}

template<typename T>
typename FibonacciHeap<T>::Node * FibonacciHeap<T>::get_min_node() {
    
    if (root == nullptr)
        throw "heap is empty";
    else
        return root;
}

template<typename T>
int FibonacciHeap<T>::get_min_key() {
    
    if (root == nullptr)
        throw "heap is empty";
    else
        return root->key;
}

template<typename T>
void FibonacciHeap<T>::remove_min() {
    
    if (root == nullptr)
        throw "heap is empty";
    
    Node * z = root;
    
    // attach all the z's children to root list
    Node * c = z->child;
    
    if (c != nullptr) {
        Node * n = c->right;
        do {
            list_insert(c, z->left, z);
            
            c->parent = nullptr;
            c = n;
            n = c->right;
        } while( c!= z->child);
    }
    
    // remove z from the root list;
    Node * l {z->left};
    Node * r {z->right};
    l->right = z->right;
    r->left = z->left;
    
    if (z == z->right) {
        // z was the only node in the root list
        root = nullptr;
    } else {
        root = z->right;
        consolidate();
    }
    --N;
    delete z;
}

/*
 *   looks two trees with the same degree. y
 */

template<typename T>
typename FibonacciHeap<T>::Node * FibonacciHeap<T>::heap_link(Node * x, Node * y) {
    
    if (root == nullptr)
        throw "heap is empty";
    
    if (y == nullptr || x == nullptr)
        throw "heap_link: one of the nodes is null";
    
    // make sure x is node with smaller key & thus future parent
    if (x->key > y->key)
        std::swap(x, y);
    
    // remove y from root list
    Node * l {y->left};
    Node * r {y->right};
    l->right = r;
    r->left = l;
    
    
    // make y a child of x
    if (x->child == nullptr) {
        
        x->child = y;
        y->left = y;
        y->right = y;
    }
    else {
        
        list_insert(y, x->child->left, x->child);
        x->child = y;
    }
    
    y->parent = x;
    y->mark = false;
    
    ++x->degree;
    
    return x;
}

/*
 *  consolidate links together trees of the same degree
 *  in the root linked-list
 */

template<typename T>
void FibonacciHeap<T>::consolidate() {
    //
    
    if (root == nullptr)
        throw "consolidate: Heap is empty";
    
    // size of the array required for consolidation
    const int D { (int) floor(log( (double) N)/log((1.0 + sqrt(5))/2.0)) };
    
    std::vector<Node * > A(D, nullptr);
    
    // pointers to all the nodes in the root list are stored in vector root_list
    // Note that relinking nodes disrupts iteration over nodes via linked linst pointers
    std::vector<Node * > root_list;
    
    root_list.reserve(N); // pick better size
    
    // populate root_list
    //      it is a bad idea to iterate over the root linked-list
    //      the index gets lost very easily as you can have first
    //      element of the look be removed from the root list
    //      And even if one uses the pointer to the following element
    //      this one can be removed as well. check out the tree in
    //      test_heap2()
    Node * w = root;
    do {
        root_list.push_back(w);
        w = w->right;
    } while (w != root);
    
    // consolidate
    for (auto w : root_list) {
        
        Node * x {w};
        int d = x->degree;
        
        assert(d < D); // if D is too small A overflows
        
        // if tree with the same degree is in A, link them
        while (A[d] != nullptr) {
            
            Node * y {A[d]};
            
            x = heap_link(x, y);  // heap_link removes node with higher key
            // from the root list
            A[d] = nullptr;
            ++d;
            
            //assert(A[d]==nullptr);  // what if x replaces existing pointer in A[d++] ?
        }
        // if A[d] is empty x goes to A[d]
        // if x is linked with tree in A[d], x has higher degree
        A[d] = x;
        
    }
    
    // reset root;
    root = nullptr;
    
    for (int i = 0; i < D; i++) {
        
        if (A[i] != nullptr) {
            
            if (root == nullptr) {
                
                // create root list with A[i] only
                A[i]->right = A[i];
                A[i]->left = A[i];
                A[i]->parent = nullptr;
                
                root = A[i];
            }
            else {
                
                list_insert(A[i], root->left, root);
                A[i]->parent = nullptr;
                
                if (A[i]->key < root->key)
                    root = A[i];
            }
        }
    }
}

template<typename T>
void FibonacciHeap<T>::decrease_key(Node * x, int k) {
    
    if (k > x->key)
        throw "new key is greater than current minimum key";
    
    x->key = k;
    Node * y {x->parent};
    
    if (y != nullptr && x->key < y->key) {
        
        cut(x,y);
        cascading_cut(y);
    }
    
    if (x->key < root->key) {
        root = x;
    }
}

/*
 *  cut removes Node x from the, y, parent's children list
 */

template<typename T>
void FibonacciHeap<T>::cut(Node * x, Node * y) {
    
    if (x->parent != y)
        throw "y is not a parent of x";
    
    // remove x from the child list of y
    if (y->child == x && x->right == x) {
        
        // x is the only child
        
        y->child = nullptr;
        y->degree = 0;
        
    }
    else {  // y has more than one child
        
        // link previous to next
        x->right->left = x->left;
        x->left->right = x->right;
        
        // if x is the first child node of y
        if (y->child == x)
            y->child = x->right;
        
        --y->degree;
    }
    
    // add x to the root list
    list_insert(x, root->left, root);
    
    x->parent = nullptr;
    x->mark = false;
    
}

/*
 *  cascading cut, remove node y from the parent's linked-list and
 *                  mark the parent node as having a child removed.
 *                  cut procedure is repeated on the parent node until
 *                  the root node is reached.
 */

template<typename T>
void FibonacciHeap<T>::cascading_cut(Node * y) {
    
    Node * z {y->parent};
    
    if (z != nullptr) {
        
        if (!y->mark) {
            
            y->mark = true;
        }
        else {
            
            cut(y, z);  // remove y from the z's child list
            cascading_cut(z);
        }
    }
}

/*
 *  remove Node
 */

template<typename T>
void FibonacciHeap<T>::remove(Node * x) {
    
    decrease_key(x, INT_MIN);
    
    remove_min();
}


template<typename T>
FibonacciHeap<T>::~FibonacciHeap() {
    
    deallocate(root);
    
}

template<typename T>
void FibonacciHeap<T>::deallocate(Node *x) {
    
    if (x == nullptr) return;
    
    // dealloc children;
    Node * c {x->child};
    
    while (c != nullptr) {
        
        Node * n {nullptr};
        
        if (c != c->right) {
            
            n = c->right;
            
            // remove from the list
            c->right->left = c->left;
            c->left->right = c->right;
        }
        
        deallocate(c);
        
        c = n;
    }
    
    delete x;
}

/*
 *  Print out and debugging functions
 */

template<typename T>
void FibonacciHeap<T>::print(int level) {
    
    std::cout << "Heap: n=" << N << " root node " << root << std::endl;
    std::cout << "root linked-list: " << std::endl;
    
    Node * node {root};
    do {
        print_node(node, 0, level, false);
        node = node->right;
    } while (node != root);
    
}


template<typename T>
void FibonacciHeap<T>::print_node(Node * n, int ntab, int level, bool print_children) {
    
    for (int i=0; i<ntab; i++) std::cout << "\t";
    
    std::cout << "node: " << n << " key " << n->key << " degree=" << n->degree << " mark=" << n->mark << std::endl;
    for (int i=0; i < ntab+1; i++) std::cout << "\t";
    
    std::cout << "children: " << std::endl;
    
    if (n->child == nullptr) {
        
        for (int i=0; i<ntab+1; i++) std::cout << "\t";
        std::cout << "none\n";
        
    } else {
        
        Node * c {n->child};
        do {
            if (ntab > level)
                
                std::cout << c << " ";
            
            else {
                
                std::cout << std::endl;
                print_node(c, ntab + 1, level);
            }
            
            c = c->right;
            
        } while (c != n->child);
        
        std::cout << std::endl;
    }
}

template<typename T>
void FibonacciHeap<T>::change_key(Node * x, int k) {
    if (x->key == k)
        throw "k is the same as x's key; that is not a key change";
    
    else if (x->key > k)
        
        decrease_key(x, k);
    
    else {
        
        // save root, the lowest node
        Node * lowest {root};
        
        // decrease x's key to -inf
        decrease_key(x, INT_MIN);
        
        //update x's key to a new key
        x->key = k;
        
        // restore minimum
        root = lowest;
        
        // attach all x's children to root list if any
        Node * c = x->child;
        
        if (c != nullptr) {
            Node * n = c->right;
            do {
                list_insert(c, x->left, x);
                
                c->parent = nullptr;
                c = n;
                n = c->right;
            } while( c!= x->child);
        }
        
        x->child = nullptr;
        x->degree = 0;
    }
}



// explicit instantiations
template struct FibonacciHeap<int>;
