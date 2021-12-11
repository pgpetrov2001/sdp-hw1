
template <typename T>
forward_list<T>::forward_list() : data(1) {}

template <typename T>
void forward_list<T>::push_back(const T& el) {
    data.push_back({el, nullptr});
    if (tail) {
        tail->next = &data.back();
    } else {
        head = &data.back();
    }
    tail = &data.back();
}

template <typename T>
size_t forward_list<T>::size() const {
    return size_;
}

template <typename T>
bool forward_list<T>::empty() const {
    return size_;
}


//**************
//iterator related
//**************

/* template <typename T> */
/* using iter = typename forward_list<T>::const_iterator; */

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::erase(forward_list<T>::const_iterator it) {
    assert(it.curr != nullptr);
    if (it.curr == head) {
        head = it.curr->next;
        return begin();
    }
    if (it.curr == tail) {
        tail = it.prev;
        if (tail) {
            tail->next = nullptr;
        }
        return end();
    }
    // if it's neither head nor tail then it is necessary if it is a valid iterator
    assert(it.prev); 
    it.prev->next = it.curr->next;
    return {it.curr->next, it.curr->next->next};
}

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::begin() const {
    return {nullptr, head};
}

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::end() const {
    return {tail, nullptr};
}

template <typename T>
const T& forward_list<T>::const_iterator::operator*() const {
    return curr->value;
}

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::const_iterator::operator++() {
    auto prevprev = prev;
    prev = curr;
    curr = curr->next;
    return {prevprev, prev};
}

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::const_iterator::operator++(int) {
    prev = curr;
    curr = curr->next;
    return *this;
}

template <typename T>
bool operator==(typename forward_list<T>::const_iterator it1, typename forward_list<T>::const_iterator it2) {
    return it1.curr == it2.curr;
}

template <typename T>
bool operator!=(typename forward_list<T>::const_iterator it1, typename forward_list<T>::const_iterator it2) {
    return it1.curr != it2.curr;
}
