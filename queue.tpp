#include "queue.h"

template <typename T>
queue<T>::queue() : data(1) {}

template <typename T>
void queue<T>::push(const T& el) {
    if (size() > data.size()) {
        data.resize(data.size() * 2);
    }
    data[tail++] = el;
}

template <typename T>
void queue<T>::pop() {
    ++head;
}

template <typename T>
T& queue<T>::front() {
    return data[head];
}

template <typename T>
const T& queue<T>::front() const {
    return data[head];
}

//demo for why size() works even when tail < head:
//000000011011 = head-tail
//111111100101 = tail-head
//000000100000 = cap
//-------11011 = (head - tail) & (cap - 1)
//    +
//-------00101 = (tail - head) & (cap - 1)
//    =
//------100000 = cap
// therefore (tail - head) & (cap - 1) = used; even when tail < head
template <typename T>
uint64_t queue<T>::size() const {
    int64_t cap = data.size();
    return uint64_t(tail - head) & uint64_t(cap - 1);
    //neither implementation defined behavior nor undefined behavior
    //because fixed-width types are guranteed to use 2s complement representation of negative numbers
}

template <typename T>
bool queue<T>::empty() const {
    return head == tail;
}
