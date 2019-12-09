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

typedef std::allocator<cache_manager::stored_node<std::pair<int, int>>> testAlloc;

TEMPLATE_TEST_CASE_SIG("limited unordered map lookup test", "[extended containers][limited containers][unordered map]",
                       ((size_t capacity), capacity), 1024)
{
    unordered_map<int, int, std::hash<int>, std::equal_to<int>,
            cache_manager::weight<std::pair<int, int>>,
            cache_manager::policy::lru<std::pair<int, int>>> map(capacity);
    size_t count = GENERATE_ANY(size_t) % (capacity - 512) + 1 + 511;
    while (map.size() < count)
    {
        map.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
    }
    size_t pos = GENERATE_ANY(size_t)%(count - 1);
    auto r_it = map.replacement_begin();
    std::advance(r_it, pos);
    auto it = map.quiet_find(r_it->first);
    REQUIRE(it != map.end());
    REQUIRE(r_it != --map.replacement_end());
    auto prev = r_it;
    --prev;
    auto next = r_it;
    ++next;
    int newKey;
    while (map.contains((newKey = GENERATE_ANY(int))));
    SECTION("Find")
    {
        SECTION("Map iter")
        {
            SECTION("Regular")
            {
                SECTION("Exists")
                {
                    auto res = map.find(it->first);
                    REQUIRE(res == it);
                    REQUIRE(*res == *it);
                    REQUIRE(++decltype(prev)(prev) == next);
                    REQUIRE(--map.replacement_end() == r_it);
                }
                SECTION("Not exists")
                {
                    auto res = map.find(newKey);
                    REQUIRE(res == map.end());
                }
            }
            SECTION("Quiet")
            {
                SECTION("Not const")
                {
                    SECTION("Exists")
                    {
                        auto res = map.quiet_find(it->first);
                        REQUIRE(res == it);
                        REQUIRE(*res == *it);
                        REQUIRE(++decltype(prev)(prev) == r_it);
                        REQUIRE(++decltype(r_it)(r_it) == next);
                    }
                    SECTION("Not exists")
                    {
                        auto res = map.quiet_find(newKey);
                        REQUIRE(res == map.end());
                    }
                }
                SECTION("Const")
                {
                    const decltype(map)& cmap = map;
                    SECTION("Exists")
                    {
                        auto res = cmap.quiet_find(it->first);
                        REQUIRE(res == it);
                        REQUIRE(*res == *it);
                        REQUIRE(++decltype(prev)(prev) == r_it);
                        REQUIRE(++decltype(r_it)(r_it) == next);
                    }
                    SECTION("Not exists")
                    {
                        auto res = cmap.quiet_find(newKey);
                        REQUIRE(res == cmap.end());
                    }
                }
            }
        }
        SECTION("Replacement iter")
        {
            SECTION("Regular")
            {
                SECTION("Exists")
                {
                    auto res = map.replacement_find(it->first);
                    REQUIRE(res == r_it);
                    REQUIRE(*res == *it);
                    REQUIRE(++decltype(prev)(prev) == next);
                    REQUIRE(--map.replacement_end() == r_it);
                }
                SECTION("Not exists")
                {
                    auto res = map.replacement_find(newKey);
                    REQUIRE(res == map.replacement_end());
                }
            }
            SECTION("Quiet")
            {
                SECTION("Not const")
                {
                    SECTION("Exists")
                    {
                        auto res = map.quiet_replacement_find(it->first);
                        REQUIRE(res == r_it);
                        REQUIRE(*res == *it);
                        REQUIRE(++decltype(prev)(prev) == r_it);
                        REQUIRE(++decltype(r_it)(r_it) == next);
                    }
                    SECTION("Not exists")
                    {
                        auto res = map.quiet_replacement_find(newKey);
                        REQUIRE(res == map.replacement_end());
                    }
                }
                SECTION("Const")
                {
                    const decltype(map)& cmap = map;
                    SECTION("Exists")
                    {
                        auto res = cmap.quiet_replacement_find(it->first);
                        REQUIRE(res == r_it);
                        REQUIRE(*res == *it);
                        REQUIRE(++decltype(prev)(prev) == r_it);
                        REQUIRE(++decltype(r_it)(r_it) == next);
                    }
                    SECTION("Not exists")
                    {
                        auto res = cmap.quiet_replacement_find(newKey);
                        REQUIRE(res == cmap.replacement_end());
                    }
                }
            }
        }
    }
    SECTION("Count")
    {
        SECTION("Regular")
        {
            SECTION("Exists")
            {
                REQUIRE(map.count(it->first) == 1);
                REQUIRE(++decltype(prev)(prev) == next);
                REQUIRE(--map.replacement_end() == r_it);
            }
            SECTION("Not exists")
            {
                REQUIRE(map.count(newKey) == 0);
            }
        }
        SECTION("Quiet")
        {
            SECTION("Exists")
            {
                REQUIRE(map.quiet_count(it->first) == 1);
                REQUIRE(++decltype(prev)(prev) == r_it);
                REQUIRE(++decltype(r_it)(r_it) == next);
            }
            SECTION("Not exists")
            {
                REQUIRE(map.quiet_count(newKey) == 0);
            }
        }
    }
    SECTION("Contains")
    {
        SECTION("Regular")
        {
            SECTION("Exists")
            {
                REQUIRE(map.contains(it->first));
                REQUIRE(++decltype(prev)(prev) == next);
                REQUIRE(--map.replacement_end() == r_it);
            }
            SECTION("Not exists")
            {
                REQUIRE_FALSE(map.contains(newKey));
            }
        }
        SECTION("Quiet")
        {
            SECTION("Exists")
            {
                REQUIRE(map.quiet_contains(it->first));
                REQUIRE(++decltype(prev)(prev) == r_it);
                REQUIRE(++decltype(r_it)(r_it) == next);
            }
            SECTION("Not exists")
            {
                REQUIRE_FALSE(map.quiet_contains(newKey));
            }
        }
    }
    SECTION("Equal range")
    {
        SECTION("Regular")
        {
            SECTION("Exists")
            {
                auto res = map.equal_range(it->first);
                REQUIRE(res.first == it);
                REQUIRE(res.second == ++decltype(it)(it));
                REQUIRE(*res.first == *it);
//                REQUIRE(++decltype(prev)(prev) == next);
                REQUIRE(--map.replacement_end() == r_it);
            }
            SECTION("Not exists")
            {
                auto res = map.equal_range(newKey);
                REQUIRE(res.first == map.end());
                REQUIRE(res.second == map.end());
            }
        }
        SECTION("Quiet")
        {
            SECTION("Not const")
            {
                SECTION("Exists")
                {
                    auto res = map.quiet_equal_range(it->first);
                    REQUIRE(res.first == it);
                    REQUIRE(res.second == ++decltype(it)(it));
                    REQUIRE(*res.first == *it);
                    REQUIRE(++decltype(prev)(prev) == r_it);
                    REQUIRE(++decltype(r_it)(r_it) == next);
                }
                SECTION("Not exists")
                {
                    auto res = map.quiet_equal_range(newKey);
                    REQUIRE(res.first == map.end());
                    REQUIRE(res.second == map.end());
                }
            }
            SECTION("Const")
            {
                const decltype(map)& cmap = map;
                SECTION("Exists")
                {
                    auto res = cmap.quiet_equal_range(it->first);
                    REQUIRE(res.first == it);
                    REQUIRE(res.second == ++decltype(it)(it));
                    REQUIRE(*res.first == *it);
                    REQUIRE(++decltype(prev)(prev) == r_it);
                    REQUIRE(++decltype(r_it)(r_it) == next);
                }
                SECTION("Not exists")
                {
                    auto res = cmap.quiet_equal_range(newKey);
                    REQUIRE(res.first == cmap.end());
                    REQUIRE(res.second == cmap.end());
                }
            }
        }
    }
    SECTION("Operator []")
    {
        SECTION("Copy")
        {
            SECTION("Exists")
            {
                SECTION("Copy")
                {
                    auto &res = map[it->first];
                    REQUIRE(&res == &it->second);
                    REQUIRE(++decltype(prev)(prev) == next);
                    REQUIRE(--map.replacement_end() == r_it);
                }
                SECTION("Move")
                {
                    auto &res = map[int(it->first)];
                    REQUIRE(&res == &it->second);
                    REQUIRE(++decltype(prev)(prev) == next);
                    REQUIRE(--map.replacement_end() == r_it);
                }
            }
            SECTION("Not exists")
            {
                SECTION("Can fit")
                {
                    SECTION("Copy")
                    {
                        std::pair<int, int> val {newKey, GENERATE_ANY(int)};
                        map[val];
                        REQUIRE(!map.empty());
                        REQUIRE(map.weight() == count + 1);
                        REQUIRE(map.capacity() == capacity);
                        REQUIRE(std::distance(map.begin(), map.end()) == count + 1);
                        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count + 1);
                        REQUIRE((--map.replacement_cend())->first == newKey);
                        REQUIRE((--map.replacement_cend())->second == val.second);
                        REQUIRE(map[newKey] == val.second);
                        REQUIRE(map.size() == count + 1);
                        REQUIRE(map.quiet_count(newKey) == 1);
                    }
                    SECTION("Move")
                    {
                        int val = GENERATE_ANY(int);
                        map[{newKey, val}];
                        REQUIRE(!map.empty());
                        REQUIRE(map.weight() == count + 1);
                        REQUIRE(map.capacity() == capacity);
                        REQUIRE(std::distance(map.begin(), map.end()) == count + 1);
                        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count + 1);
                        REQUIRE((--map.replacement_cend())->first == newKey);
                        REQUIRE((--map.replacement_cend())->second == val);
                        REQUIRE(map[newKey] == val);
                        REQUIRE(map.size() == count + 1);
                        REQUIRE(map.quiet_count(newKey) == 1);
                    }
                }
                SECTION("Needs to free")
                {
                    while(map.size() < map.capacity())
                    {
                        auto val = GENERATE_ANY(int);
                        if(val != newKey)
                        {
                            map.emplace(val, GENERATE_ANY(int));
                        }
                    }
                    auto after_rem = ++map.replacement_begin();
                    SECTION("Copy")
                    {
                        std::pair<int, int> val {newKey, GENERATE_ANY(int)};
                        map[val];
                        REQUIRE(!map.empty());
                        REQUIRE(map.weight() == capacity);
                        REQUIRE(map.capacity() == capacity);
                        REQUIRE(std::distance(map.begin(), map.end()) == capacity);
                        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == capacity);
                        REQUIRE((--map.replacement_cend())->first == newKey);
                        REQUIRE((--map.replacement_cend())->second == val.second);
                        REQUIRE(map[newKey] == val.second);
                        REQUIRE(map.size() == capacity);
                        REQUIRE(map.quiet_count(newKey) == 1);
                        REQUIRE(map.replacement_begin() == after_rem);
                    }
                    SECTION("Move")
                    {
                        int val = GENERATE_ANY(int);
                        REQUIRE_FALSE(map.quiet_contains(newKey));
                        map[{newKey, val}];
                        REQUIRE(!map.empty());
                        REQUIRE(map.weight() == capacity);
                        REQUIRE(map.capacity() == capacity);
                        REQUIRE(std::distance(map.begin(), map.end()) == capacity);
                        REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == capacity);
                        REQUIRE((--map.replacement_cend())->first == newKey);
                        REQUIRE((--map.replacement_cend())->second == val);
                        REQUIRE(map[newKey] == val);
                        REQUIRE(map.size() == capacity);
                        REQUIRE(map.quiet_count(newKey) == 1);
                        REQUIRE(map.replacement_begin() == after_rem);
                    }
                }
                SECTION("No space")
                {
                    unordered_map<int, int> map1{0};
                    SECTION("Copy")
                    {
                        REQUIRE_THROWS_AS(map1[newKey], no_space_error);
                        REQUIRE(map1.empty());
                    }
                    SECTION("Move")
                    {
                        REQUIRE_THROWS_AS(map1[int(newKey)], no_space_error);
                        REQUIRE(map1.empty());
                    }

                }
            }
        }
    }
    SECTION("At")
    {
        SECTION("Regular")
        {
            SECTION("Exists")
            {
                auto & res = map.at(it->first);
                REQUIRE(&res == &it->second);
                REQUIRE(++decltype(prev)(prev) == next);
                REQUIRE(--map.replacement_end() == r_it);
            }
            SECTION("Not exists")
            {
                REQUIRE_THROWS_AS(map.at(newKey), std::out_of_range);
            }
        }
        SECTION("Quiet")
        {
            SECTION("Not const")
            {
                SECTION("Exists")
                {
                    auto & res = map.quiet_at(it->first);
                    REQUIRE(&res == &it->second);
                    REQUIRE(++decltype(prev)(prev) == r_it);
                    REQUIRE(++decltype(r_it)(r_it) == next);
                }
                SECTION("Not exists")
                {
                    REQUIRE_THROWS_AS(map.quiet_at(newKey), std::out_of_range);
                }
            }
            SECTION("Const")
            {
                const decltype(map)& cmap = map;
                SECTION("Exists")
                {
                    auto & res = cmap.quiet_at(it->first);
                    REQUIRE(&res == &it->second);
                    REQUIRE(++decltype(prev)(prev) == r_it);
                    REQUIRE(++decltype(r_it)(r_it) == next);
                }
                SECTION("Not exists")
                {
                    REQUIRE_THROWS_AS(cmap.quiet_at(newKey), std::out_of_range);
                }
            }
        }
    }
    SECTION("Assign")
    {
        unordered_map<int, int, testHasher, testEqual, std::function<size_t(const std::pair<int, int>&)>, cache_manager::policy::lru<std::pair<int, int>>>
        map(capacity, 1, testHasher(), testEqual(), [](const std::pair<int, int>& val){return val.second;});
        int val= GENERATE_COPY(take(1, random(1, int(capacity/8))));
        while(map.can_fit(val))
        {
            int key;
            do
            {
                key = GENERATE_ANY(int);
            }
            while(map.contains(key));
            map.emplace(key, val);
            val = GENERATE_COPY(take(1, random(1, int(capacity/8))));
        }
        auto offset = GENERATE_REF(take(1, random(size_t(0), map.size() - 2)));
        auto r_pos = std::next(map.replacement_begin(), offset);
        auto r_prev = r_pos;
        --r_prev;
        auto r_next = r_pos;
        ++r_next;

        auto key = *r_pos;
        auto position = map.quiet_find(key.first);

        REQUIRE(r_next != map.replacement_end());
        REQUIRE(position != map.end());
        SECTION("Assign smaller or equal")
        {
            int newVal = GENERATE_REF(take(1, random(0, key.second)));
            REQUIRE_NOTHROW(map.assign(position, newVal));
            REQUIRE(map[key.first] == newVal);
            REQUIRE(std::next(r_prev) == r_next);
            REQUIRE(std::prev(r_next) == r_prev);
            REQUIRE(std::prev(map.replacement_end()) == r_pos);
        }
        SECTION("Assign larger")
        {
            SECTION("Can fit")
            {
                int newVal = GENERATE_REF(take(1, random(key.second + 1, int(map.capacity() - map.weight() + key.second))));
                REQUIRE_NOTHROW(map.assign(position, newVal));
                REQUIRE(map[key.first] == newVal);
                REQUIRE(std::next(r_prev) == r_next);
                REQUIRE(std::prev(r_next) == r_prev);
                REQUIRE(std::prev(map.replacement_end()) == r_pos);
            }
            SECTION("Needs to free")
            {
                int newVal = int(
                        GENERATE_REF(take(1, random(map.capacity() - map.weight() + key.second + 1, map.capacity()))));
                REQUIRE_NOTHROW(map.assign(position, newVal));
                REQUIRE(map[key.first] == newVal);
                REQUIRE(std::prev(map.replacement_end()) == r_pos);
            }
            SECTION("Can't fit")
            {
                int newVal = int(
                        GENERATE_REF(take(1, random(map.capacity() + 1, map.capacity()*2))));
                REQUIRE_THROWS_AS(map.assign(position, newVal), no_space_error);
                REQUIRE(map.quiet_at(key.first) == key.second);
                REQUIRE(std::next(r_prev) == r_pos);
                REQUIRE(std::prev(r_pos) == r_prev);
                REQUIRE(std::next(r_pos) == r_next);
                REQUIRE(std::prev(r_next) == r_pos);
            }
        }
    }
}
