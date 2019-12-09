// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#include "../unordered_map.h"
#include <unordered_set>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define GENERATE_ONE(a, b) GENERATE(take(1, random(a, b)))
#define GENERATE_ANY(type) GENERATE(take(1, random(std::numeric_limits<type>::min(), std::numeric_limits<type>::max())))

using namespace extended_containers::limited;

struct testHasher : public std::hash<int>
{};

struct testEqual : public std::equal_to<int>
{};

struct testWeight : public cache_manager::weight<std::pair<int, int>>
{};

struct testPolicy : public cache_manager::policy::lru<std::pair<int, int>>
{};

typedef std::allocator<std::pair<int, int>> testAlloc;

TEMPLATE_TEST_CASE_SIG("limited unordered map erase test", "[extended containers][limited containers][unordered map]",
                       ((size_t capacity), capacity), 1024)
{
    unordered_map<int, int> map(capacity);
    size_t count = GENERATE_ONE(size_t(capacity/2), capacity);
    while(map.size() < count)
    {
        map.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
    }
    size_t start = GENERATE_ANY(size_t)%(count/2) + 1;
    size_t eraseCount = GENERATE_ANY(size_t)%(count/2) + 1;
    SECTION("By iterator")
    {
        auto prev = map.cbegin();
        std::advance(prev, start - 1);
        auto it = prev;
        ++it;
        auto next = it;
        ++next;
        REQUIRE(map.erase(it) == next);
        REQUIRE(!map.empty());
        REQUIRE(map.weight() == count - 1);
        REQUIRE(map.capacity() == capacity);
        REQUIRE(std::distance(map.begin(), map.end()) == count - 1);
        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - 1);
        REQUIRE(map.size() == count - 1);
        REQUIRE(++prev == next);
    }
    SECTION("By replacement iterator")
    {
        auto prev = map.replacement_cbegin();
        std::advance(prev, start - 1);
        auto it = prev;
        ++it;
        auto next = it;
        ++next;
        REQUIRE(map.erase(it) == next);
        REQUIRE(!map.empty());
        REQUIRE(map.weight() == count - 1);
        REQUIRE(map.capacity() == capacity);
        REQUIRE(std::distance(map.begin(), map.end()) == count - 1);
        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - 1);
        REQUIRE(map.size() == count - 1);
        REQUIRE(++decltype(prev)(prev) == next);
        REQUIRE(--decltype(next)(next) == prev);
    }
    SECTION("By key")
    {
        SECTION("Key exists")
        {
            auto prev = map.cbegin();
            std::advance(prev, start - 1);
            auto it = prev;
            ++it;
            auto next = it;
            ++next;
            REQUIRE(map.erase(it->first) == 1);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count - 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count - 1);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - 1);
            REQUIRE(map.size() == count - 1);
            REQUIRE(++prev == next);
        }

        SECTION("Key does not exist")
        {
            int key;
            do
            {
                key = GENERATE_ANY(int);
            }
            while(map.contains(key));
            REQUIRE(map.erase(key) == 0);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
        }
    }
    SECTION("Extract by iterator")
    {
        auto prev = map.cbegin();
        std::advance(prev, start - 1);
        auto it = prev;
        ++it;
        auto next = it;
        ++next;
        auto element = *it;
        auto nh = map.extract(it);
        REQUIRE(!nh.empty());
        REQUIRE(nh.key() == element.first);
        REQUIRE(nh.mapped() == element.second);
        REQUIRE(!map.empty());
        REQUIRE(map.weight() == count - 1);
        REQUIRE(map.capacity() == capacity);
        REQUIRE(std::distance(map.begin(), map.end()) == count - 1);
        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - 1);
        REQUIRE(map.size() == count - 1);
        REQUIRE(++decltype(prev)(prev) == next);
    }
    SECTION("Extract by key")
    {
        SECTION("Key exists")
        {
            auto prev = map.cbegin();
            std::advance(prev, start - 1);
            auto it = prev;
            ++it;
            auto next = it;
            ++next;
            auto element = *it;
            auto nh = map.extract(it->first);
            REQUIRE(!nh.empty());
            REQUIRE(nh.key() == element.first);
            REQUIRE(nh.mapped() == element.second);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count - 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count - 1);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - 1);
            REQUIRE(map.size() == count - 1);
            REQUIRE(++prev == next);
        }

        SECTION("Key does not exist")
        {
            int key;
            do
            {
                key = GENERATE_ANY(int);
            }
            while(map.contains(key));
            auto nh = map.extract(key);
            REQUIRE(nh.empty());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
        }
    }
    SECTION("Range by iterators")
    {
        auto s_prev = map.cbegin();
        std::advance(s_prev, start - 1);
        auto s_it = s_prev;
        ++s_it;
        auto s_next = s_it;
        ++s_next;
        auto e_prev = s_it;
        std::advance(e_prev, eraseCount - 1);
        auto e_it = e_prev;
        ++e_it;

        SECTION("Empty")
        {
            REQUIRE(map.erase(s_it, s_it) == s_it);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(++decltype(s_prev)(s_prev) == s_it);
            REQUIRE(++decltype(s_it)(s_it) == s_next);
        }
        SECTION("Not empty")
        {
            REQUIRE(map.erase(s_it, e_it) == e_it);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count - eraseCount);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count - eraseCount);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - eraseCount);
            REQUIRE(map.size() == count - eraseCount);
            REQUIRE(++decltype(s_prev)(s_prev) == e_it);
        }
    }
    SECTION("Range by replacement iterators")
    {
        auto s_prev = map.replacement_cbegin();
        std::advance(s_prev, start - 1);
        auto s_it = s_prev;
        ++s_it;
        auto s_next = s_it;
        ++s_next;
        auto e_prev = s_it;
        std::advance(e_prev, eraseCount - 1);
        auto e_it = e_prev;
        ++e_it;

        SECTION("Empty")
        {
            REQUIRE(map.erase(s_it, s_it) == s_it);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(++decltype(s_prev)(s_prev) == s_it);
            REQUIRE(++decltype(s_it)(s_it) == s_next);
            REQUIRE(--decltype(s_next)(s_next) == s_it);
            REQUIRE(--decltype(s_it)(s_it) == s_prev);
        }
        SECTION("Not empty")
        {
            REQUIRE(map.erase(s_it, e_it) == e_it);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count - eraseCount);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count - eraseCount);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count - eraseCount);
            REQUIRE(map.size() == count - eraseCount);
            REQUIRE(++decltype(s_prev)(s_prev) == e_it);
            REQUIRE(--decltype(e_it)(e_it) == s_prev);
        }
    }
    SECTION("Clear")
    {
        REQUIRE_NOTHROW(map.clear());
        REQUIRE(map.empty());
        REQUIRE(map.weight() == 0);
        REQUIRE(map.capacity() == capacity);
        REQUIRE(std::distance(map.begin(), map.end()) == 0);
        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 0);
        REQUIRE(map.size() == 0);
    }
}
