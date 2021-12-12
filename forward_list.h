#include <vector>
#include <cstdint>
#include <cassert>
/* #include <memory> */

#include <iostream>
using std::endl;

using std::vector;
using std::size_t;

#ifndef LIST
#define LIST

//one important difference to std::forward_list
//is that it doesn't destroy elements that are erased
//!!!!but it destroys all elements that have been added at one point in its destructor(the implicitly defined one)
//i know it may take up a lot of excessive memory that way when a lot of elements are added and erased
//but it is an interesting implementation, that is faster if you don't care too much about unused memory like is the case in this assignment
//and makes use of rule of zero, so no need to implement copy and move constructors and assigment operators

template <typename T>
class forward_list {
    struct node {
        T value;
        node *next;
        /* node *prev; */
    };
public:
    class const_iterator { //!!!! will not use pass by reference for this class because it is 2 pointers anyway
    public:
        friend class forward_list;
        const T &operator*() const {
            assert(curr != nullptr);
            return curr->value;
        }
        bool operator==(typename forward_list<T>::const_iterator it) const {
            if (curr == it.curr) assert(prev == it.prev);
            return curr == it.curr;
        }
        bool operator!=(typename forward_list<T>::const_iterator it) const {
            if (curr != it.curr) assert(prev != it.prev || prev == nullptr);
            return curr != it.curr;
        }
        const_iterator operator++();
        const_iterator operator++(int);

    private:
        const_iterator(node *prev, node *curr) : prev(prev), curr(curr) {}
        node *prev;
        node *curr;
    };

    forward_list();
    forward_list(const forward_list &);
    void push_back(const T& el);
    const_iterator erase(const_iterator it);

    const_iterator begin() const;
    const_iterator end() const;
    size_t size() const;
    bool empty() const;

private:
    /* unique_ptr<node> head; */
    /* unique_ptr<node> tail; */
    vector<node> data;
    // this way it preserves locality and is easier to implement
    // although it doesn't destroy erased elements
    node *head = nullptr;
    node *tail = nullptr;
    size_t size_ = 0;
};


#include "forward_list.tpp"

#endif
