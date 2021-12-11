#include <vector>
#include <cstdint>

using std::int64_t; //specified by standard that uses 2s complement
//that's why it is used instead of size_t
using std::uint64_t;

#ifndef QUEUE
#define QUEUE

template <typename T>
class queue {
public:
    queue();
    void push(const T &el);
    void pop();
    T &front();
    const T &front() const;
    uint64_t size() const;
    bool empty() const;
private:
    void increment(int64_t &what) const;
    std::vector<T> data;
    int64_t head = 0;
    int64_t tail = 0;
};

#include "queue.tpp"

#endif
