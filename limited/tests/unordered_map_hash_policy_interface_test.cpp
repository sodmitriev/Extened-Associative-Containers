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

TEMPLATE_TEST_CASE_SIG("limited unordered map hash policy interface test", "[extended containers][limited containers][unordered map]",
                       ((size_t capacity), capacity), 1024)
{
    std::unordered_map<int, int> map(capacity);
    size_t count = GENERATE_ONE(size_t(2), capacity);
    while(map.size() < count)
    {
        map.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
    }
    auto initBucketCount = map.bucket_count();
    SECTION("Rehash")
    {
        map.rehash(initBucketCount + GENERATE_ONE(size_t(1), capacity));
        REQUIRE(map.bucket_count() > initBucketCount);
    }
    SECTION("Reserve")
    {
        map.reserve(size_t(float(initBucketCount)*map.max_load_factor()) + GENERATE_ONE(size_t(1), capacity));
        REQUIRE(map.bucket_count() > initBucketCount);
    }
    SECTION("Set max load factor")
    {
        auto newLoadFactor = (map.load_factor() * 3) / 4;
        map.max_load_factor(newLoadFactor);
        REQUIRE(map.max_load_factor() == newLoadFactor);
        while(!map.insert({GENERATE_ANY(int), GENERATE_ANY(int)}).second);
        REQUIRE(map.bucket_count() > initBucketCount);
    }
}
