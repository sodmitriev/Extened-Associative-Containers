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

TEMPLATE_TEST_CASE_SIG("limited unordered map iterators test", "[extended containers][limited containers][unordered map]",
          ((size_t capacity), capacity), 1024)
{
    unordered_map<int, int> map(capacity);
    size_t count = GENERATE_ONE(capacity/2, capacity);
    while (map.size() < count)
    {
        map.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
    }
    SECTION("Regular")
    {
        SECTION("Begin - end")
        {
            REQUIRE(std::distance(map.begin(), map.end()) == count);
        }
        SECTION("Begin const - end const")
        {
            const decltype(map)& cmap = map;
            REQUIRE(std::distance(cmap.begin(), cmap.end()) == count);
        }
        SECTION("Cbegin - cend")
        {
            REQUIRE(std::distance(map.cbegin(), map.cend()) == count);
        }
    }
    SECTION("Replacement")
    {
        SECTION("Begin - end")
        {
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
        }
        SECTION("Begin const - end const")
        {
            const decltype(map)& cmap = map;
            REQUIRE(std::distance(cmap.replacement_begin(), cmap.replacement_end()) == count);
        }
        SECTION("Cbegin - cend")
        {
            REQUIRE(std::distance(map.replacement_cbegin(), map.replacement_cend()) == count);
        }
    }
    SECTION("Bucket")
    {
        size_t pos = GENERATE_REF(take(1, random(size_t(0), count - 1)));
        auto it = map.begin();
        std::advance(it, pos);
        auto bucket = map.bucket(it->first);
        SECTION("Begin - end")
        {
            REQUIRE(std::distance(map.begin(bucket), map.end(bucket)) == map.bucket_size(bucket));
        }
        SECTION("Begin const - end const")
        {
            const decltype(map)& cmap = map;
            REQUIRE(std::distance(cmap.begin(bucket), cmap.end(bucket)) == cmap.bucket_size(bucket));
        }
        SECTION("Cbegin - cend")
        {
            REQUIRE(std::distance(map.cbegin(bucket), map.cend(bucket)) == map.bucket_size(bucket));
        }
    }
}
