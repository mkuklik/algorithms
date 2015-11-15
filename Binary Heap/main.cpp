#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

/*
 *  Bineary Heap
 *
 *  Created by mkuklik on 11/8/15.
 *  Copyright © 2015 mkuklik. All rights reserved.
 */

/*
operation linked list binary heap
MAKE-HEAP       O(1)    O(1)
ISEMPTY         O(1)    O(1)
INSERT          O(1)    O(log n)
EXTRACT-MIN     O(n)    O(log n)
DECREASE-KEY    O(1)    O(log n)
DELETE          O(1)    O(log n)
MELD            O(1)    O(n)
FIND-MIN        O(n)    O(1)
*/

// insert
// 1. place element at the end of the array,
// 2. percolate up until in right place

// delete minimum
// 1. pick minimum element from the top of the heap
// 2. replace it with the last one
// 3. percolate it down to restore the heap

// decrease key
// 1. decrease key of the element in the tree
// 2. percolate up

// increase key
// 1. increase the key and
// 2. percolate down

// delete any element
// 1. decrease key to -Inf
// 2. remove the minimum element

//
// Meld. Given two binary heaps H1 and H2, merge into a single binary heap.

/* Build Max-Heap : Using MAX-HEAPIFY() we can construct a max-heap by starting with the last node that has children (which occurs at A.length/2 the elements the array A.(length/2+1) to A.n are all leaves of the tree ) and iterating back to the root calling MAX-HEAPIFY() for each node which ensures that the max-heap property will be maintained at each step for all evaluated nodes. The pseudocode for the routine is

    BUILD-MAX-HEAP(A)
        A.heapsize = A.length
        for i = A.length/2 downto 1
            MAX-HEAPIFY(A,i)
*/

// 0 , 1, 2 , (3, 4) , (5, 6), 7, 8

//using namespace std;

inline size_t parent(size_t i) { return (i - (size_t) 1) >> 1; }
inline size_t left_child(size_t i) {return 2*i+1;}
inline size_t right_child(size_t i) {return 2*(i+1);}

/*
 *  percolateDown, moves the node from to-down
 *  until the heap properties are restored
 *
 *  used when adding new element
 */

template<typename T>
void percolateDown(std::vector<T> &a, size_t i) {
    
    auto s = a.size();
    size_t left {left_child(i)};
    size_t right {right_child(i)};
    
    size_t top {i};
    
    if (left < s && a[top] < a[left])
        top = left;
    
    if (right < s && a[top] < a[right])
        top = right;
    
    if (top != i) {
        std::swap(a[top], a[i]);
        percolateDown(a, top);
    }
    
}

/*
 *  percolateUp, moves the node from bottom-up
 *  until the heap properties are restored
 *
 *  used when adding new element
 */

template<typename T>
void percolateUp(std::vector<T> &a, size_t i) {
    
    if (i == 0) return;
    
    for (; i != 0;) {
        
        size_t j = parent(i);
        
        if (a[j] > a[i]) break;
        
        std::swap(a[j], a[i]);

        i = j;
    }
}

/*
 *  create heap in the vector a, in place;
 */

template<class T>
void heapify(std::vector<T> &a) {
    
    size_t s = a.size();
    
    if (s < 2) return;
    
    for (size_t i = parent(s-1) + 1; i != 0; ) {
        percolateDown(a, --i);
    }
    
}


/*
 *  Main
 */

int main(int argc, const char * argv[]) {

    std::vector<int> x1(10);
    generate(x1.begin(), x1.end(), []() -> int { return std::rand()%100; });
    
    for (auto x: x1) std::cout << x << " ";
    std::cout << std::endl;
    
    heapify<int>(x1);
    
    for (auto x: x1) std::cout << x << " ";
    std::cout << std::endl;
    
    x1[7] = 100;
    percolateUp(x1, 7);
    
    for (auto x: x1) std::cout << x << " ";
    std::cout << std::endl;

}
