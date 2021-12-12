#define CATCH_CONFIG_MAIN

#include <vector>

#include "queue.h"
#include "forward_list.h"
#include "catch.hpp"

TEST_CASE("push and pop 1 element") {
    queue<int> q;
    q.push(2);
    REQUIRE(q.front() == 2);
    REQUIRE(q.size() == 1);
    q.pop();
    REQUIRE(q.size() == 0);
    REQUIRE(q.empty());
}

TEST_CASE("cyclic queue: push n elements, pop n-1 elements, push n elements, check") {
    queue<int> q;

    //shift the head a little bit
    for (int i=0; i<99; i++) {
        q.push(i);
        if (i == 20) {
            q.pop();
            q.pop();
            q.pop();
        }
    }
    while (!q.empty()) {
        q.pop();
    }

    //then do actual test
    for (int i=0; i<127; i++) {
        q.push(i);
    }
    REQUIRE(q.front() == 0);
    REQUIRE(q.size() == 127);
    for (int i=0; i<126; i++) {
        q.pop();
        REQUIRE(q.size() == 126-i);
        REQUIRE(q.front() == i+1);
    }
    for (int i=0; i<127; i++) {
        q.push(i);
        REQUIRE(q.front() == 126);
        REQUIRE(q.size() == i+2);
    }
    q.push(1234);
    for (int i=0; i<127; i++) {
        q.pop();
        REQUIRE(q.front() == i);
        REQUIRE(q.size() == 128-i);
    }
    q.pop();
    REQUIRE(q.size() == 1);
    REQUIRE(q.front() == 1234);
}

TEST_CASE("forward_list: push n elements, pop n-1 elements, push n elements, check") {
    forward_list<int> q;

    //shift the head a little bit
    for (int i=0; i<99; i++) {
        q.push_back(i);
        if (i == 20) {
            q.erase(q.begin());
            q.erase(q.begin());
            q.erase(q.begin());
        }
    }
    while (!q.empty()) {
        q.erase(q.begin());
    }

    //then do actual test
    for (int i=0; i<127; i++) {
        q.push_back(i);
    }
    REQUIRE(*q.begin() == 0);
    REQUIRE(q.size() == 127);
    for (int i=0; i<126; i++) {
        q.erase(q.begin());
        REQUIRE(q.size() == 126-i);
        REQUIRE(*q.begin() == i+1);
    }
    for (int i=0; i<127; i++) {
        q.push_back(i);
        REQUIRE(*q.begin() == 126);
        REQUIRE(q.size() == i+2);
    }
    q.push_back(1234);
    for (int i=0; i<127; i++) {
        q.erase(q.begin());
        REQUIRE(*q.begin() == i);
        REQUIRE(q.size() == 128-i);
    }
    q.erase(q.begin());
    REQUIRE(q.size() == 1);
    REQUIRE(*q.begin() == 1234);
}

TEST_CASE("asdas") {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.pop();
    q.pop();
    REQUIRE(q.empty());
}

TEST_CASE("asdasdsada") {
    queue<int> q;
    q.push(1);
    q.pop();
    q.push(1);
    assert(!q.empty());
}

TEST_CASE("iterate list with 2 elemeents") {
    forward_list<int> l;
    l.push_back(1);
    l.push_back(2);
    int i = 1;
    for (auto it = l.begin(); it != l.end(); ++it) {
        REQUIRE(*it == i);
        i++;
    }
}

TEST_CASE("dunno") {
    forward_list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.erase(l.begin());
    for (auto el : l) {
        REQUIRE(el == 2);
    }
}

TEST_CASE("asdasd") {
    forward_list<int> l;
    l.push_back(2);
    l.push_back(3);
    l.push_back(4);
    l.push_back(5);
    auto it = l.begin();
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(*it == 3);
    ++it;
    REQUIRE(*it == 4);
    it = l.erase(it);
    REQUIRE(*it == 5);
    l.push_back(6);
    ++it;
    REQUIRE(*it == 6);
    l.push_back(7);
    it = l.erase(it);
    REQUIRE(*it == 7);
    l.push_back(8);
    l.push_back(9);
    l.push_back(10);
    it = l.begin();
    REQUIRE(*(it++) == 2);
    REQUIRE(*(it++) == 3);
    REQUIRE(*(it++) == 5);
    REQUIRE(*(it++) == 7);
    REQUIRE(*(it++) == 8);
    REQUIRE(*(it++) == 9);
    REQUIRE(*(it++) == 10);
}

TEST_CASE("asdasdasdsadasd") {
    forward_list<int> l;
    l.push_back(2);
    l.push_back(3);
    REQUIRE(*l.begin() == 2);
    l.erase(l.begin());
    REQUIRE(*l.begin() == 3);
}
