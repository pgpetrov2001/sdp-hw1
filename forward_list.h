#include <vector>
#include <cstdint>
/* #include <memory> */

using std::vector;
using std::size_t;

#ifndef LIST
#define LIST

//one important difference to std::forward_list
//is that it doesn't destroy elements that are erased
//!!!!but it destroys all elements that have been added at one point
//in its destructor(the implicitly defined one)

template <typename T>
class forward_list {
    struct node {
        T value;
        node *next;
        /* node *prev; */
    };
public:
    class const_iterator {
    public:
        friend class forward_list;
        // this is to emulate behavior of stl:
        /* const_iterator() = delete; */
        const T &operator*() const;
        //operator ->
        template <typename U>
        friend bool operator==(const_iterator, const_iterator);
        template <typename U>
        friend bool operator!=(const_iterator, const_iterator);
        const_iterator operator++();
        const_iterator operator++(int);

    private:
        const node *prev;
        const node *curr;
    };

    forward_list();
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


template <typename T>
bool operator==(typename forward_list<T>::const_iterator, typename forward_list<T>::const_iterator);
template <typename T>
bool operator!=(typename forward_list<T>::const_iterator, typename forward_list<T>::const_iterator);

#include "forward_list.tpp"

#endif
