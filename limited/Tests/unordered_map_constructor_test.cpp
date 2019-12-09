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

TEST_CASE("limited unordered map trivial constructors test", "[extended containers][limited containers][unordered map]")
{
    SECTION("Empty")
    {
        unordered_map<int, int> map;
        REQUIRE(map.empty());
        REQUIRE(map.weight() == 0);
        REQUIRE(map.capacity() == 0);
        REQUIRE(map.begin() == map.end());
        REQUIRE(map.replacement_begin() == map.replacement_end());
        REQUIRE(map.size() == 0);
        REQUIRE(map.load_factor() == 0);
        REQUIRE(map.max_size() > 0);
        REQUIRE(map.max_load_factor() > 0);
        REQUIRE(map.max_bucket_count() > 0);
    }
    SECTION("With parameters")
    {
        SECTION("capacity")
        {
            auto capacity = GENERATE_ANY(size_t);
            unordered_map<int, int> map(capacity);
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
        }
        SECTION("capacity + bucket_count")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int> map(capacity, bucketCount);
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
        }
        SECTION("capacity + bucket_count + hasher")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher> map
                    (capacity, bucketCount, testHasher());
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
        }
        SECTION("capacity + bucket_count + hasher + key_equal")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual> map
                    (capacity, bucketCount, testHasher(), testEqual());
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto equal = map.key_eq();
            REQUIRE(std::is_same_v<decltype(equal), testEqual>);
        }
        SECTION("capacity + bucket_count + hasher + key_equal + weigher")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight> map
                    (capacity, bucketCount, testHasher(), testEqual(), testWeight());
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto equal = map.key_eq();
            REQUIRE(std::is_same_v<decltype(equal), testEqual>);
            auto weigher = map.weigher();
            REQUIRE(std::is_same_v<decltype(weigher), testWeight>);
        }
        SECTION("capacity + bucket_count + hasher + key_equal + weigher + policy")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy> map
                    (capacity, bucketCount, testHasher(), testEqual(), testWeight(), testPolicy());
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto equal = map.key_eq();
            REQUIRE(std::is_same_v<decltype(equal), testEqual>);
            auto weigher = map.weigher();
            REQUIRE(std::is_same_v<decltype(weigher), testWeight>);
            auto policy = map.policy();
            REQUIRE(std::is_same_v<decltype(policy), testPolicy>);
        }
        SECTION("capacity + bucket_count + hasher + key_equal + weigher + policy + allocator")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map
                    (capacity, bucketCount, testHasher(), testEqual(), testWeight(), testPolicy(), testAlloc());
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto equal = map.key_eq();
            REQUIRE(std::is_same_v<decltype(equal), testEqual>);
            auto weigher = map.weigher();
            REQUIRE(std::is_same_v<decltype(weigher), testWeight>);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
        SECTION("capacity + allocator")
        {
            auto capacity = GENERATE_ANY(size_t);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map(capacity,
                                                                                                  testAlloc());
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
        SECTION("allocator")
        {
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map{testAlloc{}};
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == 0);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
        SECTION("capacity + bucket_count + allocator")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map
                    (capacity, bucketCount, testAlloc{});
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
        SECTION("capacity + bucket_count + hasher + allocator")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map
                    (capacity, bucketCount, testHasher{}, testAlloc{});
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
        SECTION("capacity + bucket_count + hasher + key_equal + allocator")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map
                    (capacity, bucketCount, testHasher{}, testEqual{}, testAlloc{});
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto equal = map.key_eq();
            REQUIRE(std::is_same_v<decltype(equal), testEqual>);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
        SECTION("capacity + bucket_count + hasher + key_equal + weigher + allocator")
        {
            auto capacity = GENERATE_ANY(size_t);
            auto bucketCount = GENERATE_ONE(1, 512);
            unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map
                    (capacity, bucketCount, testHasher{}, testEqual{}, testWeight{}, testAlloc{});
            REQUIRE(map.empty());
            REQUIRE(map.weight() == 0);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(map.begin() == map.end());
            REQUIRE(map.replacement_begin() == map.replacement_end());
            REQUIRE(map.size() == 0);
            REQUIRE(map.load_factor() == 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            //May be larger as it rounds up to nearest prime number
            REQUIRE(map.bucket_count() >= bucketCount);
            auto hasher = map.hash_function();
            REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
            auto equal = map.key_eq();
            REQUIRE(std::is_same_v<decltype(equal), testEqual>);
            auto weigher = map.weigher();
            REQUIRE(std::is_same_v<decltype(weigher), testWeight>);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
        }
    }
}
