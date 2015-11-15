//
//  Fibonacci Heap Implementation
//
//  Created by Michael Kuklik on 11/12/15.
//  Copyright © 2015 Michael Kuklik. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

struct FibonacciHeap {
    
    struct Node {
        /* each node can have multiple children at any level of the tree, 
         * but it has one pointer to only one child. Each node has pointer 
         * to parent. Each node that pointers to two sieblings, next and previous.
         */
        int key;
        
        bool mark{false};
        /* The boolean-valued attribute x:mark indicates whether node x has 
         * lost a child since the last time x was made the child of another node. 
         */
        
        Node* child{nullptr};
        int degree {0}; // rank, number of children in the childrent linked-list
        
        Node* left{this};
        Node* right{this};
        
        Node* parent{nullptr};
        
        Node(int k): key(k) {};
    };
    
    int N{0};                   // number of nodes in the heap
    Node* root {nullptr};       // points to the root list with lowest key

    /*
     *  Methods
     */
    
    ~FibonacciHeap();
    void deallocate(Node *n);
    void list_insert(Node* l, Node* r, Node* x);
    Node * heap_link(Node* y, Node* x);   // links two trees from the root linked list
    void consolidate();
    void cut(Node* x, Node* y);
    void cascading_cut(Node* y);
    Node * insert(int k);       // insert key into the heap
    int get_min();              // get the lowest key
    void remove_min();          // remove the lowest key
    void decrease_key(Node* x, int k);
    void change_key(Node * x, int k);
    void remove(Node * x);      // remove Node
    
    /* priting/debugging functions */
    
    void print_node(Node* n, int ntab, int level, bool print_children);
    void print(int level);
};

/*
 *  insert a node in between note l (on the left) and r (on the right)
 *        in the linked list
 */

void FibonacciHeap::list_insert(Node* x, Node* l, Node* r) {
// insert Node x between node l and r in double-linked list
    
    x->left = l;
    x->right = r;
    
    r->left = x;
    l->right = x;
}

/*
 *  insert new node with key k and value v
 */

FibonacciHeap::Node* FibonacciHeap::insert(int k) {

    Node* tmp = new Node(k);
    
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
    

int FibonacciHeap::get_min() {
    
    if (root == nullptr)
        throw "heap is empty";
    else
        return root->key;
}

void FibonacciHeap::remove_min() {
    
    if (root == nullptr)
        throw "heap is empty";
        
    Node* z = root;
    
    // attach all the z's children to root list
    Node* c = z->child;
    
    if (c != nullptr) {
        Node* n = c->right;
        do {
            list_insert(c, z->left, z);
            
            c->parent = nullptr;
            c = n;
            n = c->right;
        } while( c!= z->child);
    }
    
    // remove z from the root list;
    Node* l {z->left};
    Node* r {z->right};
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

FibonacciHeap::Node * FibonacciHeap::heap_link(Node* x, Node* y) {
    
    if (root == nullptr)
        throw "heap is empty";
    
    if (y == nullptr || x == nullptr)
        throw "heap_link: one of the nodes is null";
    
    // make sure x is node with smaller key & thus future parent
    if (x->key > y->key)
        std::swap(x, y);
    
    // remove y from root list
    Node* l {y->left};
    Node* r {y->right};
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
    
void FibonacciHeap::consolidate() {
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
        
        Node* x {w};
        int d = x->degree;
        
        assert(d < D); // if D is too small A overflows
        
        // if tree with the same degree is in A, link them
        while (A[d] != nullptr) {
            
            Node* y {A[d]};
            
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


void FibonacciHeap::decrease_key(Node* x, int k) {
    
    if (k > x->key)
        throw "new key is greater than current minimum key";
    
    x->key = k;
    Node* y {x->parent};
    
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

void FibonacciHeap::cut(Node * x, Node * y) {
        
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

void FibonacciHeap::cascading_cut(Node* y) {

    Node* z {y->parent};
    
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
    
void FibonacciHeap::remove(Node * x) {

    decrease_key(x, INT_MIN);

    remove_min();
}



FibonacciHeap::~FibonacciHeap() {
    
    deallocate(root);
    
}

void FibonacciHeap::deallocate(Node *x) {
    
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

    
void FibonacciHeap::print(int level) {

    std::cout << "Heap: n=" << N << " root node " << root << std::endl;
    std::cout << "root linked-list: " << std::endl;
    
    Node * node {root};
    do {
        print_node(node, 0, level, false);
        node = node->right;
    } while (node != root);
    
}
    
void FibonacciHeap::print_node(Node* n, int ntab, int level, bool print_children=false) {
    
    for (int i=0; i<ntab; i++) std::cout << "\t";
    
    std::cout << "node: " << n << " key " << n->key << " degree=" << n->degree << " mark=" << n->mark << std::endl;
    for (int i=0; i < ntab+1; i++) std::cout << "\t";
    
    std::cout << "children: " << std::endl;
    
    if (n->child == nullptr) {
        
        for (int i=0; i<ntab+1; i++) std::cout << "\t";
        std::cout << "none\n";
        
    } else {
        
        Node* c {n->child};
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

void FibonacciHeap::change_key(Node * x, int k) {
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
        Node* c = x->child;
        
        if (c != nullptr) {
            Node* n = c->right;
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

// test\debug functions listed below
void test_heap1();
void test_heap2();
void test_heap3();
void sort_test(FibonacciHeap &h);


/*
 *  Main
 */

int main(int argc, const char * argv[]) {
    // insert code here...

    // NOTE: in test_heap 1,2,3, nodes are allocated outside
    // of the FibonacciHeap but FibonacciHeap internal functions
    // deallocate those nodes; thus Valgrind shows lost records
    
    // min extraction, consolidation & key decrease debugging
    test_heap1();
    
    // consolidation loop debugging and expermients
    test_heap2();
    
    // testing/debugging increase key
    test_heap3();
    
    // overall test, heap sort
    FibonacciHeap hsort;
    sort_test(hsort);
    
    return 0;
}













/*******************************************************************
 *
 *  methods below are artificially created trees
 *  See figure 19.4 from CLRS p 514
 *
 */


void test_heap1() {
    
    FibonacciHeap h;
    
    // Replication of figure 19.4 from CLRS p 514
    std::vector<FibonacciHeap::Node *> nodes;  // vector with pointer to all nodes
    
    int k[] {23, 7, 21, 3, 17, 24};
    int d[] {0, 0, 0, 3, 1, 2};
    
    for (int i=0; i<6; i++) {
        
        FibonacciHeap::Node * n = new FibonacciHeap::Node(k[i]);
        nodes.push_back(n);
    }
    
    for (int i=0; i<6; i++) {
        nodes[i]->degree = d[i];
        
        if (i==0) nodes[i]->left = nodes[5];
        else nodes[i]->left = nodes[i-1];
        
        if (i==5) nodes[i]->right = nodes[0];
        else nodes[i]->right = nodes[i+1];
    }
    
    h.root = nodes[3];
    
    
    // k=3
    int kk[] {18, 52, 38};
    int dd[] {1,0,1};
    for (int i=0; i<3; i++) {
        FibonacciHeap::Node * n = new FibonacciHeap::Node(kk[i]);
        nodes.push_back(n);
        n->parent = nodes[3];
        n->degree = dd[i];
        if (kk[i] == 18) n->mark = true;
    }
    for (int i=6; i<9; i++) {
        if (i==6) nodes[i]->left = nodes[8];
        else nodes[i]->left = nodes[i-1];
        
        if (i==8) nodes[i]->right=nodes[6];
        else nodes[i]->right = nodes[i+1];
    }
    
    nodes[3]->child = nodes[6];
    
    // 39,  nodes[9];
    FibonacciHeap::Node * n = new FibonacciHeap::Node(39);
    nodes.push_back(n);
    n->parent = nodes[6];
    n->right = n;
    n->left = n;
    n->mark = true;
    nodes[6]->child = n;
    
    // 41
    n = new FibonacciHeap::Node(41);
    nodes.push_back(n);
    n->parent = nodes[8];
    n->right = n;
    n->left = n;
    n->mark = false;
    nodes[8]->child = n;
    
    FibonacciHeap::Node * n41 {n};
    
    // 30
    n = new FibonacciHeap::Node(30);
    nodes.push_back(n);
    n->parent = nodes[4];
    n->right = n;
    n->left = n;
    n->mark = false;
    nodes[4]->child = n;
    
    // 26
    n = new FibonacciHeap::Node(26);
    nodes.push_back(n);
    n->parent = nodes[5];
    n->right = n;
    n->left = n;
    n->mark = true;
    n->degree = 1;
    nodes[5]->child = n;
    
    // 35
    FibonacciHeap::Node * nn = new FibonacciHeap::Node(35);
    nodes.push_back(nn);
    nn->parent = n;
    nn->right = nn;
    nn->left = nn;
    nn->mark = false;
    n->child = nn;
    
    FibonacciHeap::Node * n35 {nn};
    
    // 46
    nn = new FibonacciHeap::Node(46);
    nodes.push_back(nn);
    nn->parent = nodes[5];
    nn->right = n;
    nn->left = n;
    nn->mark = false;
    
    FibonacciHeap::Node * n46 {nn};
    
    n->left = nn;
    n->right = nn;
    
    h.N = 15;
    
    h.print(5);
    
    // Extact minimum
    h.remove_min();
    
    std::cout << "\nmin removed\n\n";
    h.print(5);
    
    h.decrease_key(n46, 15);      // remove 46
    
    std::cout << "\ndecrease_key 46 -> 15\n\n";
    
    h.print(5);
    
    h.decrease_key(n35, 5);       //
    
    std::cout << "\ndecrease_key 35 -> 5\n\n";
    
    h.print(5);
    
    h.remove(n41);
    
    std::cout << "\n41 removed\n\n";
    
    h.print(5);
    
}

/*
 *      testing consolidation loop, testing point
 *      root points to the
 */

void test_heap2() {
    
    FibonacciHeap h;
    
    // Replication of figure 19.4 from CLRS p 514
    std::vector<FibonacciHeap::Node *> nodes;  // vector with pointer to all nodes
    
    int k[] {23, 21, 7, 3, 17, 24};
    int d[] {0, 0, 0, 3, 1, 2};
    
    for (int i=0; i<6; i++) {
        
        FibonacciHeap::Node * n = new FibonacciHeap::Node(k[i]);
        nodes.push_back(n);
    }
    
    for (int i=0; i<6; i++) {
        nodes[i]->degree = d[i];
        
        if (i==0) nodes[i]->left = nodes[5];
        else nodes[i]->left = nodes[i-1];
        
        if (i==5) nodes[i]->right = nodes[0];
        else nodes[i]->right = nodes[i+1];
    }
    
    // k=3
    int kk[] {18, 52, 38};
    int dd[] {1,0,1};
    for (int i=0; i<3; i++) {
        FibonacciHeap::Node * n = new FibonacciHeap::Node(kk[i]);
        nodes.push_back(n);
        n->parent = nodes[3];
        n->degree = dd[i];
        if (kk[i] == 18) n->mark = true;
    }
    for (int i=6; i<9; i++) {
        if (i==6) nodes[i]->left = nodes[8];
        else nodes[i]->left = nodes[i-1];
        
        if (i==8) nodes[i]->right=nodes[6];
        else nodes[i]->right = nodes[i+1];
    }
    
    nodes[3]->child = nodes[6];
    
    // 39,  nodes[9];
    FibonacciHeap::Node * n = new FibonacciHeap::Node(39);
    nodes.push_back(n);
    n->parent = nodes[6];
    n->right = n;
    n->left = n;
    n->mark = true;
    nodes[6]->child = n;
    
    // 41
    n = new FibonacciHeap::Node(41);
    nodes.push_back(n);
    n->parent = nodes[8];
    n->right = n;
    n->left = n;
    n->mark = false;
    nodes[8]->child = n;
    
    // 30
    n = new FibonacciHeap::Node(30);
    nodes.push_back(n);
    n->parent = nodes[4];
    n->right = n;
    n->left = n;
    n->mark = false;
    nodes[4]->child = n;
    
    // 26
    n = new FibonacciHeap::Node(26);
    nodes.push_back(n);
    n->parent = nodes[5];
    n->right = n;
    n->left = n;
    n->mark = true;
    n->degree = 1;
    nodes[5]->child = n;
    
    // 35
    FibonacciHeap::Node * nn = new FibonacciHeap::Node(35);
    nodes.push_back(nn);
    nn->parent = n;
    nn->right = nn;
    nn->left = nn;
    nn->mark = false;
    n->child = nn;
    
    // 46
    nn = new FibonacciHeap::Node(46);
    nodes.push_back(nn);
    nn->parent = nodes[5];
    nn->right = n;
    nn->left = n;
    nn->mark = false;
    
    n->left = nn;
    n->right = nn;
    
    h.N = 15;
    
    h.root = nodes[0];    // this is not the minimum element
    // this only for testing loop over
    // root nodes in consolidate()
    
    h.print(5);
    
    h.remove_min();
    
    h.print(5);
}


void test_heap3() {
    
    FibonacciHeap h;
    
    // Replication of figure 19.4 from CLRS p 514
    std::vector<FibonacciHeap::Node *> nodes;  // vector with pointer to all nodes
    
    int k[] {23, 7, 21, 3, 17, 24};
    int d[] {0, 0, 0, 3, 1, 2};
    
    for (int i=0; i<6; i++) {
        
        FibonacciHeap::Node * n = new FibonacciHeap::Node(k[i]);
        nodes.push_back(n);
    }
    
    for (int i=0; i<6; i++) {
        nodes[i]->degree = d[i];
        
        if (i==0) nodes[i]->left = nodes[5];
        else nodes[i]->left = nodes[i-1];
        
        if (i==5) nodes[i]->right = nodes[0];
        else nodes[i]->right = nodes[i+1];
    }
    
    h.root = nodes[3];
    
    
    // k=3
    int kk[] {18, 52, 38};
    int dd[] {1,0,1};
    for (int i=0; i<3; i++) {
        FibonacciHeap::Node * n = new FibonacciHeap::Node(kk[i]);
        nodes.push_back(n);
        n->parent = nodes[3];
        n->degree = dd[i];
        if (kk[i] == 18) n->mark = true;
    }
    for (int i=6; i<9; i++) {
        if (i==6) nodes[i]->left = nodes[8];
        else nodes[i]->left = nodes[i-1];
        
        if (i==8) nodes[i]->right=nodes[6];
        else nodes[i]->right = nodes[i+1];
    }
    
    nodes[3]->child = nodes[6];
    
    // 39,  nodes[9];
    FibonacciHeap::Node * n = new FibonacciHeap::Node(39);
    nodes.push_back(n);
    n->parent = nodes[6];
    n->right = n;
    n->left = n;
    n->mark = true;
    nodes[6]->child = n;
    
    // 41
    n = new FibonacciHeap::Node(41);
    nodes.push_back(n);
    n->parent = nodes[8];
    n->right = n;
    n->left = n;
    n->mark = false;
    nodes[8]->child = n;
    
    // 30
    n = new FibonacciHeap::Node(30);
    nodes.push_back(n);
    n->parent = nodes[4];
    n->right = n;
    n->left = n;
    n->mark = false;
    nodes[4]->child = n;
    
    // 26
    n = new FibonacciHeap::Node(26);
    nodes.push_back(n);
    n->parent = nodes[5];
    n->right = n;
    n->left = n;
    n->mark = true;
    n->degree = 1;
    nodes[5]->child = n;
    
    // 35
    FibonacciHeap::Node * nn = new FibonacciHeap::Node(35);
    nodes.push_back(nn);
    nn->parent = n;
    nn->right = nn;
    nn->left = nn;
    nn->mark = false;
    n->child = nn;
    
    // 46
    nn = new FibonacciHeap::Node(46);
    nodes.push_back(nn);
    nn->parent = nodes[5];
    nn->right = n;
    nn->left = n;
    nn->mark = false;
    
    n->left = nn;
    n->right = nn;
    
    h.N = 15;
    
    h.remove_min();
    
    h.print(5);
    
    // Extact minimum
    h.change_key(nodes[6], 40);
    
    std::cout << "\n increased 18 to 40\n\n";
    
    h.print(5);
    
    h.remove_min();
    
    std::cout << "\nremove min\n\n";
    
    h.print(5);
    
    // take off remaining keys
    for (int i=0; i < 13; i++) {
        std::cout << h.get_min() << " ";
        h.remove_min();
    }
    std::cout << "\n";
}


/*
 *      testing consolidation loop, testing point
 */

void sort_test(FibonacciHeap &h) {
    
    // Sort test!
    int N = 100;
    std::vector<int> a;
    
    for (int i=0; i < N; i++) {
        int tmp = std::rand()%1000;
        a.push_back(tmp);
        h.insert(tmp);
    }
    
    h.consolidate();
    h.print(10);
    
    int last = INT_MIN;
    
    for (int i = 0; i < N; i++) {
        if (last > h.get_min()) std::cout << "SNAFU \n\n\n";
        
        std::cout << h.get_min() << " ";
        last = h.get_min();
        h.remove_min();
    }
    
}

