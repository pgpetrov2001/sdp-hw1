
template <typename T>
forward_list<T>::forward_list() : data(1) {}

template <typename T>
forward_list<T>::forward_list(const forward_list<T> &other) {
    /* data.reserve(other.size()); */
    assert(head == nullptr && tail == nullptr && size() == 0);
    for (const auto &element : other) {
        push_back(element);
    }
    assert(size() == other.size());
}

template <typename T>
void forward_list<T>::push_back(const T& el) {
    vector<T> elements;
    for (auto e : *this) {
        elements.push_back(e);
    }
    data.push_back({el, nullptr});
    if (tail) {
        tail->next = &data.back();
    } else {
        assert(head == nullptr);
        head = &data.back();
        assert(head != nullptr);
    }
    tail = &data.back();
    ++size_;
    assert(elements.size() + 1 == size());
    auto it = begin();
    for (auto e : elements) {
        assert(it != end());
        assert(e == *it);
        ++it;
    }
    assert(it != end());
    assert(*it == el);
    ++it;
    assert(it == end());
}

template <typename T>
size_t forward_list<T>::size() const {
    return size_;
}

template <typename T>
bool forward_list<T>::empty() const {
    return size_ == 0;
}


//**************
//iterator related
//**************

/* template <typename T> */
/* using iter = typename forward_list<T>::const_iterator; */
template<typename T>
void check(vector<T> with, const forward_list<T> &l, T val) {
    assert(with.size() == l.size() + 1);
    auto it1 = with.begin();
    auto it2 = l.begin();
    while (*it1 == *it2) {
        ++it1;
        ++it2;
    }
    assert(*it1 == val);
    ++it1;
    while (it1 != with.end()) {
        assert(it2 != l.end());
        assert(*it1 == *it2);
    }
}

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::erase(forward_list<T>::const_iterator it) {
    vector<T> elements;
    for (const auto &el : *this) {
        elements.push_back(el);
    }
    T erasedval = it.curr->value;
    assert(!empty());
    assert(it.curr != nullptr);
    --size_;
    if (empty()) {
        head = tail = nullptr;
        check(elements, *this, erasedval);
        return end();
    }
    if (it.curr == head) {
        head = head->next;
        check(elements, *this, erasedval);
        return begin();
    }
    if (it.curr == tail) {
        tail = it.prev;
        assert(tail != nullptr);
        tail->next = nullptr;
        check(elements, *this, erasedval);
        return end();
    }
    // if it's neither head nor tail then it is necessary if it is a valid iterator
    assert(it.prev); 
    assert(it.curr->next != nullptr);
    it.prev->next = it.curr->next;
    check(elements, *this, erasedval);
    return {it.prev, it.curr->next};
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
typename forward_list<T>::const_iterator forward_list<T>::const_iterator::operator++(int) {
    auto prevprev = prev;
    prev = curr;
    curr = curr->next;
    return {prevprev, prev};
}

template <typename T>
typename forward_list<T>::const_iterator forward_list<T>::const_iterator::operator++() {
    prev = curr;
    curr = curr->next;
    return *this;
}

