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

struct simpleWeight
{
    size_t operator()(const std::pair<int, int>& val) const
    {
        return size_t(val.second%10000);
    }
};

struct simpleLocker
{
    bool operator()(const std::pair<int, int>& it) const
    {
        return it.second>10000;
    }
};

typedef std::allocator<cache_manager::stored_node<std::pair<int, int>>> testAlloc;

TEMPLATE_TEST_CASE_SIG("limited unordered map replacement interface test", "[extended containers][limited containers][unordered map]",
          ((size_t capacity), capacity), 1024)
{
    size_t locked = 0;
    unordered_map<int, int, testHasher, testEqual, simpleWeight,
            cache_manager::policy::locked_policy<std::pair<int, int>, cache_manager::policy::lru<std::pair<int, int>>, 
            simpleLocker>> map(capacity);
    auto count = GENERATE_ONE(size_t(2), capacity - 1);
    while (map.size() < count)
    {
        if(GENERATE_ONE(0, 1))
        {
            ++locked;
            while(!map.emplace(GENERATE_ANY(int), 10001).second);
        }
        else
        {
            while(!map.emplace(GENERATE_ANY(int), 1).second);
        }
    }
    SECTION("Can fit")
    {
        SECTION("By weight")
        {
            SECTION("Can")
            {
                REQUIRE(map.can_fit(GENERATE_REF(take(1, random(size_t(0), capacity - count - 1)))));
            }
            SECTION("Can't")
            {
                REQUIRE_FALSE(map.can_fit(GENERATE_REF(take(1, random(capacity - count + 1, capacity)))));
            }
        }
        SECTION("By object")
        {
            SECTION("Can")
            {
                REQUIRE(map.can_fit({GENERATE_ANY(int), int(GENERATE_REF(take(1, random(size_t(0), capacity - count - 1))))}));
            }
            SECTION("Can't")
            {
                REQUIRE_FALSE(map.can_fit({GENERATE_ANY(int), int(GENERATE_REF(take(1, random(capacity - count + 1, capacity))))}));
            }
        }
    }
    SECTION("Access")
    {
        size_t pos = GENERATE_REF(take(1, random(size_t(0), count - 2)));
        auto r_it = map.replacement_begin();
        std::advance(r_it, pos);
        auto it = map.quiet_find(r_it->first);
        REQUIRE(it != map.end());
        REQUIRE(r_it != --map.replacement_end());
        auto prev = r_it;
        --prev;
        auto next = r_it;
        ++next;
        SECTION("By key")
        {
            map.access(it->first);
            REQUIRE(--map.replacement_cend() == r_it);
            REQUIRE(++decltype(prev)(prev) == next);
            REQUIRE(--decltype(next)(next) == prev);
        }
        SECTION("By regular iterator")
        {
            map.access(it);
            REQUIRE(--map.replacement_cend() == r_it);
            REQUIRE(++decltype(prev)(prev) == next);
            REQUIRE(--decltype(next)(next) == prev);
        }
        SECTION("By replacement iterator")
        {
            map.access(r_it);
            REQUIRE(--map.replacement_cend() == r_it);
            REQUIRE(++decltype(prev)(prev) == next);
            REQUIRE(--decltype(next)(next) == prev);
        }
    }
    SECTION("Free space")
    {
        SECTION("Weight exceeds current")
        {
            REQUIRE_THROWS_AS(map.free_space(GENERATE_REF(take(1, random(count + 1, count * 2)))), no_space_error);
        }
        SECTION("Too many locked")
        {
            REQUIRE_THROWS_AS(map.free_space(GENERATE_REF(take(1, random(count - locked + 1, count)))), no_space_error);
        }
        SECTION("Successful")
        {
            auto toFree = GENERATE_REF(take(1, random(size_t(0), count - locked)));
            REQUIRE_NOTHROW(map.free_space(toFree));
            REQUIRE(map.size() == count - toFree);
        }
    }
    SECTION("Provide space")
    {
        SECTION("By weight")
        {
            SECTION("Ask more than capacity")
            {
                REQUIRE_THROWS_AS(map.provide_space(GENERATE_REF(take(1, random(capacity + 1, capacity * 2)))), no_space_error);
            }
            SECTION("Ask when enough free space")
            {
                REQUIRE_NOTHROW(map.provide_space(GENERATE_REF(take(1, random(size_t(1), capacity - count )))));
                REQUIRE(map.size() == count);
            }
            SECTION("Too many locked")
            {
                REQUIRE_THROWS_AS(map.provide_space(GENERATE_REF(take(1, random(count - locked + 1, count))) + (capacity - count)), no_space_error);
            }
            SECTION("Successful")
            {
                auto toFree = GENERATE_REF(take(1, random(size_t(0), count - locked)));
                REQUIRE_NOTHROW(map.provide_space(toFree + capacity - count));
                REQUIRE(map.size() == count - toFree);
            }
        }
        SECTION("By object")
        {
            decltype(map)::value_type val;
            SECTION("Ask more than capacity")
            {
                val.second = static_cast<int>(GENERATE_REF(take(1, random(capacity + 1, capacity * 2 ))));
                REQUIRE_THROWS_AS(map.provide_space(val), no_space_error);
            }
            SECTION("Ask when enough free space")
            {
                val.second = static_cast<int>(GENERATE_REF(take(1, random(size_t(1), capacity - count ))));
                REQUIRE_NOTHROW(map.provide_space(val));
                REQUIRE(map.size() == count);
            }
            SECTION("Too many locked")
            {
                val.second = static_cast<int>(GENERATE_REF(take(1, random((count - locked) + 1 + (capacity - count), capacity ))));
                REQUIRE_THROWS_AS(map.provide_space(val), no_space_error);
            }
            SECTION("Successful")
            {
                auto toFree = GENERATE_REF(take(1, random(size_t(0), count - locked)));
                val.second = static_cast<int>(toFree + capacity - count);
                REQUIRE_NOTHROW(map.provide_space(val));
                REQUIRE(map.size() == count - toFree);
            }
        }
    }
}
