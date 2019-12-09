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

struct testWeight : public cache_manager::weight<int>
{};

struct testPolicy : public cache_manager::policy::lru<std::pair<int, int>>
{};

typedef std::allocator<cache_manager::stored_node<std::pair<int, int>>> testAlloc;

TEMPLATE_TEST_CASE_SIG("limited unordered map insertion test", "[extended containers][limited containers][unordered map]",
                       ((size_t capacity), capacity), 1024)
{
    SECTION("Insert single")
    {
        unordered_map<int, int> map{capacity};
        SECTION("Map has free space")
        {
            SECTION("Copy")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert(val);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Move")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert(decltype(val)(val));
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Convertible pair")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                std::pair<long, const long> val1{val.first, val.second};
                auto res = map.insert(val1);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Copy with hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert(map.begin(), val);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Move with hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert(map.begin(), decltype(val)(val));
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Convertible pair with hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                std::pair<long, const long> val1{val.first, val.second};
                auto res = map.insert(map.begin(), val1);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Key already exists")
        {
            std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
            auto res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(*res.first == val);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == val);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == val);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            SECTION("Copy")
            {
                auto res = map.insert(val);
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Move")
            {
                auto res = map.insert(decltype(val)(val));
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Convertible pair")
            {
                std::pair<long, const long> val1{val.first, val.second};
                auto res = map.insert(val1);
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Copy with hint")
            {
                auto res = map.insert(map.begin(), val);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Move with hint")
            {
                auto res = map.insert(map.begin(), decltype(val)(val));
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("Convertible pair with hint")
            {
                std::pair<long, const long> val1{val.first, val.second};
                auto res = map.insert(map.begin(), val1);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Map needs to free")
        {
            std::unordered_set<int> used;
            std::pair<int, int> initVal{GENERATE_ANY(int), GENERATE_ANY(int)};
            map.insert(initVal);
            for (size_t i = 1; i < map.capacity(); ++i)
            {
                int val;
                do
                {
                    val = GENERATE_ANY(int);
                } while (val == initVal.first || !used.insert(val).second);
            }
            for (auto i : used)
            {
                REQUIRE(map.insert({i, GENERATE_ANY(int)}).second);
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == map.capacity());
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
            REQUIRE(map.size() == map.capacity());
            int key;
            do
            {
                key = GENERATE_ANY(int);
            } while (key == initVal.first || !used.insert(key).second);
            std::pair<int, int> val{key, GENERATE_ANY(int)};
            SECTION("Copy")
            {
                auto res = map.insert(val);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("Move")
            {
                auto res = map.insert(decltype(val)(val));
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("Convertible pair")
            {
                std::pair<long, const long> val1{val.first, val.second};
                auto res = map.insert(val1);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("Copy with hint")
            {
                auto res = map.insert(map.begin(), val);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("Move with hint")
            {
                auto res = map.insert(map.begin(), decltype(val)(val));
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("Convertible pair with hint")
            {
                std::pair<long, const long> val1{val.first, val.second};
                auto res = map.insert(map.begin(), val1);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
        }
        SECTION("Map can't fit")
        {
            unordered_map<int, int> map1{0};
            SECTION("Copy")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert(val), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("Move")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert(decltype(val)(val)), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("Convertible pair")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                std::pair<long, const long> val1{val.first, val.second};
                REQUIRE_THROWS_AS(map1.insert(val1), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("Copy with hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert(map1.begin(), val), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("Move with hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert(map1.begin(), decltype(val)(val)), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("Convertible pair with hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                std::pair<long, const long> val1{val.first, val.second};
                REQUIRE_THROWS_AS(map1.insert(map1.begin(), val1), no_space_error);
                REQUIRE(map1.empty());
            }
        }
    }
    SECTION("Emplace")
    {
        unordered_map<int, int> map{capacity};
        SECTION("Map has free space")
        {
            SECTION("No hint")
            {
                std::pair<const int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.emplace(val.first, val.second);
                REQUIRE(res.second);
                REQUIRE(res.first->first == val.first);
                REQUIRE(res.first->second == val.second);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(map.begin()->first == val.first);
                REQUIRE(map.begin()->second == val.second);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(map.replacement_begin()->first == val.first);
                REQUIRE(map.replacement_begin()->second == val.second);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint")
            {
                std::pair<const int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.emplace_hint(map.begin(), val.first, val.second);
                REQUIRE(res->first == val.first);
                REQUIRE(res->second == val.second);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(map.begin()->first == val.first);
                REQUIRE(map.begin()->second == val.second);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(map.replacement_begin()->first == val.first);
                REQUIRE(map.replacement_begin()->second == val.second);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Key already exists")
        {
            std::pair<const int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
            auto res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(res.first->first == val.first);
            REQUIRE(res.first->second == val.second);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(map.begin()->first == val.first);
            REQUIRE(map.begin()->second == val.second);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(map.replacement_begin()->first == val.first);
            REQUIRE(map.replacement_begin()->second == val.second);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            SECTION("No hint")
            {
                auto res = map.emplace(val.first, val.second);
                REQUIRE(!res.second);
                REQUIRE(res.first->first == val.first);
                REQUIRE(res.first->second == val.second);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(map.begin()->first == val.first);
                REQUIRE(map.begin()->second == val.second);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(map.replacement_begin()->first == val.first);
                REQUIRE(map.replacement_begin()->second == val.second);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint")
            {
                auto res = map.emplace_hint(map.begin(), val.first, val.second);
                REQUIRE(res->first == val.first);
                REQUIRE(res->second == val.second);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(map.begin()->first == val.first);
                REQUIRE(map.begin()->second == val.second);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(map.replacement_begin()->first == val.first);
                REQUIRE(map.replacement_begin()->second == val.second);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Map needs to free")
        {
            std::unordered_set<int> used;
            std::pair<int, int> initVal{GENERATE_ANY(int), GENERATE_ANY(int)};
            map.insert(initVal);
            for (size_t i = 1; i < map.capacity(); ++i)
            {
                int val;
                do
                {
                    val = GENERATE_ANY(int);
                } while (val == initVal.first || !used.insert(val).second);
            }
            for (auto i : used)
            {
                REQUIRE(map.insert({i, GENERATE_ANY(int)}).second);
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == map.capacity());
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
            REQUIRE(map.size() == map.capacity());
            int key;
            do
            {
                key = GENERATE_ANY(int);
            } while (key == initVal.first || !used.insert(key).second);
            std::pair<int, int> val{key, GENERATE_ANY(int)};
            SECTION("No hint")
            {
                auto res = map.emplace(val.first, val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("With hint")
            {
                auto res = map.emplace_hint(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
        }
        SECTION("Map can't fit")
        {
            unordered_map<int, int> map1{0};
            SECTION("No hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.emplace(val.first, val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("With hint")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.emplace_hint(map1.begin(), val.first, val.second), no_space_error);
                REQUIRE(map1.empty());
            }
        }
    }
    SECTION("Try emplace")
    {
        unordered_map<int, int> map{capacity};
        SECTION("Map has free space")
        {
            SECTION("No hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.try_emplace(val.first, val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("No hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.try_emplace(int(val.first), val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.try_emplace(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.try_emplace(map.begin(), int(val.first), val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Key already exists")
        {
            std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
            auto res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(*res.first == val);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == val);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == val);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            SECTION("No hint copy key")
            {
                auto res = map.try_emplace(val.first, val.second);
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("No hint move key")
            {
                auto res = map.try_emplace(int(val.first), val.second);
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint copy key")
            {
                auto res = map.try_emplace(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint move key")
            {
                auto res = map.try_emplace(map.begin(), int(val.first), val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Map needs to free")
        {
            std::unordered_set<int> used;
            std::pair<int, int> initVal{GENERATE_ANY(int), GENERATE_ANY(int)};
            map.insert(initVal);
            for (size_t i = 1; i < map.capacity(); ++i)
            {
                int val;
                do
                {
                    val = GENERATE_ANY(int);
                } while (val == initVal.first || !used.insert(val).second);
            }
            for (auto i : used)
            {
                REQUIRE(map.insert({i, GENERATE_ANY(int)}).second);
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == map.capacity());
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
            REQUIRE(map.size() == map.capacity());
            int key;
            do
            {
                key = GENERATE_ANY(int);
            } while (key == initVal.first || !used.insert(key).second);
            std::pair<int, int> val{key, GENERATE_ANY(int)};
            SECTION("No hint copy key")
            {
                auto res = map.try_emplace(val.first, val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("No hint move key")
            {
                auto res = map.try_emplace(int(val.first), val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("With hint copy key")
            {
                auto res = map.try_emplace(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("With hint move key")
            {
                auto res = map.try_emplace(map.begin(), int(val.first), val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
        }
        SECTION("Map can't fit")
        {
            unordered_map<int, int> map1{0};
            SECTION("No hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.try_emplace(val.first, val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("No hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.try_emplace(int(val.first), val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("With hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.try_emplace(map1.begin(), val.first, val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("With hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.try_emplace(map1.begin(), int(val.first), val.second), no_space_error);
                REQUIRE(map1.empty());
            }
        }
    }
    SECTION("Insert node")
    {
        unordered_map<int, int> map{capacity};
        SECTION("Map has free space")
        {
            std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
            auto res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(*res.first == val);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == val);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == val);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            auto node = map.extract(res.first);
            REQUIRE(node.key() == val.first);
            REQUIRE(node.mapped() == val.second);
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 0);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 0);
            REQUIRE(map.size() == 0);
            REQUIRE(map.quiet_count(val.first) == 0);
            SECTION("No hint")
            {
                auto res = map.insert(std::move(node));
                REQUIRE(res.inserted);
                REQUIRE(*res.position == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint")
            {
                auto res = map.insert(map.begin(), std::move(node));
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Key already exists")
        {
            std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
            auto res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(*res.first == val);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == val);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == val);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            auto node = map.extract(res.first);
            REQUIRE(node.key() == val.first);
            REQUIRE(node.mapped() == val.second);
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 0);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 0);
            REQUIRE(map.size() == 0);
            REQUIRE(map.quiet_count(val.first) == 0);
            res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(*res.first == val);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == val);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == val);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            SECTION("No hint")
            {
                auto res = map.insert(std::move(node));
                REQUIRE(!res.inserted);
                REQUIRE(*res.position == val);
                REQUIRE(res.node.key() == val.first);
                REQUIRE(res.node.mapped() == val.second);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint")
            {
                auto res = map.insert(map.begin(), std::move(node));
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Map needs to free")
        {
            std::unordered_set<int> used;
            std::pair<int, int> initVal{GENERATE_ANY(int), GENERATE_ANY(int)};
            map.insert(initVal);
            for (size_t i = 1; i < map.capacity(); ++i)
            {
                int val;
                do
                {
                    val = GENERATE_ANY(int);
                } while (val == initVal.first || !used.insert(val).second);
            }
            for (auto i : used)
            {
                REQUIRE(map.insert({i, GENERATE_ANY(int)}).second);
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == map.capacity());
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
            REQUIRE(map.size() == map.capacity());
            int key;
            do
            {
                key = GENERATE_ANY(int);
            } while (key == initVal.first || !used.insert(key).second);
            std::pair<int, int> val{key, GENERATE_ANY(int)};
            unordered_map<int, int> map1{1};
            auto res = map1.insert(val);
            REQUIRE(res.second);
            auto node = map1.extract(res.first);
            REQUIRE(node.key() == val.first);
            REQUIRE(node.mapped() == val.second);
            SECTION("No hint")
            {
                auto res = map.insert(std::move(node));
                REQUIRE(res.inserted);
                REQUIRE(*res.position == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("With hint")
            {
                auto res = map.insert(map.begin(), std::move(node));
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
        }
        SECTION("Map can't fit")
        {
            std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
            auto res = map.insert(val);
            REQUIRE(res.second);
            REQUIRE(*res.first == val);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == val);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == val);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(val.first) == 1);
            auto node = map.extract(res.first);
            REQUIRE(node.key() == val.first);
            REQUIRE(node.mapped() == val.second);
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 0);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 0);
            REQUIRE(map.size() == 0);
            REQUIRE(map.quiet_count(val.first) == 0);
            unordered_map<int, int> map1{0};
            SECTION("No hint")
            {
                REQUIRE_THROWS_AS(map1.insert(std::move(node)), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("With hint")
            {
                REQUIRE_THROWS_AS(map1.insert(map1.begin(), std::move(node)), no_space_error);
                REQUIRE(map1.empty());
            }
        }
    }
    SECTION("Insert or assign")
    {
        unordered_map<int, int> map{capacity};
        SECTION("Map has free space")
        {
            SECTION("No hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert_or_assign(val.first, val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("No hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert_or_assign(int(val.first), val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert_or_assign(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                auto res = map.insert_or_assign(map.begin(), int(val.first), val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Key already exists")
        {
            std::pair<int, int> initVal{GENERATE_ANY(int), GENERATE_ANY(int)};
            std::pair<int, int> val{initVal.first, GENERATE_ANY(int)};
            auto res = map.insert(initVal);
            REQUIRE(res.second);
            REQUIRE(*res.first == initVal);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == 1);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == 1);
            REQUIRE(*map.begin() == initVal);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
            REQUIRE(*map.replacement_begin() == initVal);
            REQUIRE(map.size() == 1);
            REQUIRE(map.quiet_count(initVal.first) == 1);
            SECTION("No hint copy key")
            {
                auto res = map.insert_or_assign(val.first, val.second);
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("No hint move key")
            {
                auto res = map.insert_or_assign(int(val.first), val.second);
                REQUIRE(!res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint copy key")
            {
                auto res = map.insert_or_assign(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
            SECTION("With hint move key")
            {
                auto res = map.insert_or_assign(map.begin(), int(val.first), val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == 1);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == 1);
                REQUIRE(*map.begin() == val);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == 1);
                REQUIRE(*map.replacement_begin() == val);
                REQUIRE(map.size() == 1);
                REQUIRE(map.quiet_count(val.first) == 1);
            }
        }
        SECTION("Map needs to free")
        {
            std::unordered_set<int> used;
            std::pair<int, int> initVal{GENERATE_ANY(int), GENERATE_ANY(int)};
            map.insert(initVal);
            for (size_t i = 1; i < map.capacity(); ++i)
            {
                int val;
                do
                {
                    val = GENERATE_ANY(int);
                } while (val == initVal.first || !used.insert(val).second);
            }
            for (auto i : used)
            {
                REQUIRE(map.insert({i, GENERATE_ANY(int)}).second);
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == map.capacity());
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
            REQUIRE(map.size() == map.capacity());
            int key;
            do
            {
                key = GENERATE_ANY(int);
            } while (key == initVal.first || !used.insert(key).second);
            std::pair<int, int> val{key, GENERATE_ANY(int)};
            SECTION("No hint copy key")
            {
                auto res = map.insert_or_assign(val.first, val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("No hint move key")
            {
                auto res = map.insert_or_assign(int(val.first), val.second);
                REQUIRE(res.second);
                REQUIRE(*res.first == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("With hint copy key")
            {
                auto res = map.insert_or_assign(map.begin(), val.first, val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
            SECTION("With hint move key")
            {
                auto res = map.insert_or_assign(map.begin(), int(val.first), val.second);
                REQUIRE(*res == val);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == map.capacity());
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == map.capacity());
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == map.capacity());
                REQUIRE(*--map.replacement_end() == val);
                REQUIRE(map.size() == map.capacity());
                REQUIRE(map.quiet_count(val.first) == 1);
                REQUIRE(map.quiet_count(initVal.first) == 0);
            }
        }
        SECTION("Map can't fit")
        {
            unordered_map<int, int> map1{0};
            SECTION("No hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert_or_assign(val.first, val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("No hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert_or_assign(int(val.first), val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("With hint copy key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert_or_assign(map1.begin(), val.first, val.second), no_space_error);
                REQUIRE(map1.empty());
            }
            SECTION("With hint move key")
            {
                std::pair<int, int> val{GENERATE_ANY(int), GENERATE_ANY(int)};
                REQUIRE_THROWS_AS(map1.insert_or_assign(map1.begin(), int(val.first), val.second), no_space_error);
                REQUIRE(map1.empty());
            }
        }
    }
    SECTION("Insert range")
    {
        unordered_map<int, int> map{capacity};
        size_t count = GENERATE_ANY(size_t)%(capacity - 1) + 2;
        std::unordered_map<int, int> cmpMap{count};
        while(cmpMap.size() < count)
        {
            cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("Map has free space")
        {
            map.insert(cmpMap.cbegin(), cmpMap.cend());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
        }
        SECTION("Some keys already exist")
        {
            size_t usedCount = (GENERATE_ANY(size_t)%count) / 2 + 1;
            auto start = cmpMap.cbegin();
            std::advance(start, (GENERATE_ANY(size_t)%count) / 3);
            auto end = start;
            std::advance(end, usedCount);
            map.insert(start, end);
            map.insert(cmpMap.cbegin(), cmpMap.cend());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
        }
        SECTION("Map needs to free")
        {
            size_t usedCount = GENERATE_ANY(size_t)%count + capacity - count + 1;
            std::unordered_set<int> usedVals(usedCount);
            while(usedVals.size() < usedCount)
            {
                auto val = GENERATE_ANY(int);
                if(cmpMap.count(val) == 0)
                    usedVals.insert(val);
            }
            for(auto i : usedVals)
            {
                map.insert({i, GENERATE_ANY(int)});
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == usedCount);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == usedCount);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == usedCount);
            REQUIRE(map.size() == usedCount);
            map.insert(cmpMap.cbegin(), cmpMap.cend());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == capacity);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == capacity);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == capacity);
            REQUIRE(map.size() == capacity);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                auto it1 = usedVals.find(i.first);
                REQUIRE((it != cmpMap.cend() || it1 != usedVals.cend()));
                if(it != cmpMap.cend())
                {
                    REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
                }
                else
                {
                    REQUIRE(*it1 == i.first);
                }
            }
        }
        SECTION("Map can't fit")
        {
            map.insert(cmpMap.cbegin(), cmpMap.cend());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
            auto fullCount = GENERATE_ANY(size_t)%capacity + capacity + 1;
            while(cmpMap.size() < fullCount + count)
            {
                cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
            }
            REQUIRE_THROWS_AS(map.insert(cmpMap.cbegin(), cmpMap.cend()), no_space_error);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
        }
    }
    SECTION("Insert initializer list")
    {
        size_t ilistCapacity = 3;
        unordered_map<int, int> map{ilistCapacity};
        size_t count = 2;
        std::unordered_map<int, int> cmpMap{count};
        while(cmpMap.size() < count)
        {
            cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("Map has free space")
        {
            map.insert({*cmpMap.begin(), *(++cmpMap.begin())});
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == ilistCapacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
        }
        SECTION("Some keys already exist")
        {
            map.insert({*cmpMap.begin()});
            map.insert({*cmpMap.begin(), *(++cmpMap.begin())});
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == ilistCapacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
        }
        SECTION("Map needs to free")
        {
            size_t usedCount = GENERATE_ANY(size_t)%count + ilistCapacity - count + 1;
            std::unordered_set<int> usedVals(usedCount);
            while(usedVals.size() < usedCount)
            {
                auto val = GENERATE_ANY(int);
                if(cmpMap.count(val) == 0)
                    usedVals.insert(val);
            }
            for(auto i : usedVals)
            {
                map.insert({i, GENERATE_ANY(int)});
            }
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == usedCount);
            REQUIRE(map.capacity() == ilistCapacity);
            REQUIRE(std::distance(map.begin(), map.end()) == usedCount);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == usedCount);
            REQUIRE(map.size() == usedCount);
            map.insert({*cmpMap.begin(), *(++cmpMap.begin())});
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == ilistCapacity);
            REQUIRE(map.capacity() == ilistCapacity);
            REQUIRE(std::distance(map.begin(), map.end()) == ilistCapacity);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == ilistCapacity);
            REQUIRE(map.size() == ilistCapacity);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                auto it1 = usedVals.find(i.first);
                REQUIRE((it != cmpMap.cend() || it1 != usedVals.cend()));
                if(it != cmpMap.cend())
                {
                    REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
                }
                else
                {
                    REQUIRE(*it1 == i.first);
                }
            }
        }
        SECTION("Map can't fit")
        {
            map.insert({*cmpMap.begin(), *(++cmpMap.begin())});
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == ilistCapacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
            auto fullCount = 4;
            while(cmpMap.size() < fullCount + count)
            {
                cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
            }
            REQUIRE_THROWS_AS(map.insert(
                    {*cmpMap.begin(), *(++cmpMap.begin()), *(++++cmpMap.begin()), *(++++++cmpMap.begin()),
                     *(++++++++cmpMap.begin()), *(++++++++++cmpMap.begin())}
            ), no_space_error);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == ilistCapacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            for(auto & i : map)
            {
                auto it = cmpMap.find(i.first);
                REQUIRE(it != cmpMap.cend());
                REQUIRE(it->first == i.first);
                REQUIRE(it->second == i.second);
            }
        }
    }
}
