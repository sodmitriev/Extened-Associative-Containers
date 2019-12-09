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

TEMPLATE_TEST_CASE_SIG("limited unordered map fill, copy and move test", "[extended containers][limited containers][unordered map]",
                       ((size_t capacity), capacity), 1024)
{
    SECTION("Filling constructors")
    {
        auto bucketCount = GENERATE_ONE(1, 512);
        size_t count = GENERATE(take(1, random(size_t(2), capacity)));
        std::unordered_map<int, int> cmpMap{count};
        while(cmpMap.size() < count)
        {
            cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("Iterators + capacity")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int> map(cmpMap.cbegin(), cmpMap.cend(), capacity);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int>> map;
                REQUIRE_THROWS_AS((map = std::make_unique<decltype(map)::element_type>(cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))))), no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int> map(cmpMap.cbegin(), cmpMap.cend(), capacity, bucketCount);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int>> map;
                REQUIRE_THROWS_AS((map = std::make_unique<decltype(map)::element_type>(cmpMap.cbegin(), cmpMap.cend(),
                                                                                       GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount)), no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher> map(cmpMap.cbegin(), cmpMap.cend(), capacity, bucketCount, testHasher());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount, testHasher())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + key_equal")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual> map(cmpMap.cbegin(), cmpMap.cend(), capacity,
                                                                   bucketCount, testHasher(), testEqual());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                auto equal = map.key_eq();
                REQUIRE(std::is_same_v<decltype(equal), testEqual>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                    bucketCount, testHasher(), testEqual())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + key_equal + weigher")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight> map(cmpMap.cbegin(), cmpMap.cend(), capacity,
                                                                               bucketCount, testHasher(), testEqual(), testWeight());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                    bucketCount, testHasher(), testEqual(), testWeight())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + key_equal + weigher + policy")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy> map(cmpMap.cbegin(), cmpMap.cend(), capacity,
                                                                                           bucketCount, testHasher(), testEqual(),
                                                                                           testWeight(), testPolicy());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                    bucketCount, testHasher(), testEqual(), testWeight(), testPolicy())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + key_equal + weigher + policy + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map(cmpMap.cbegin(), cmpMap.cend(), capacity,
                                                                                                      bucketCount, testHasher(), testEqual(),
                                                                                                      testWeight(), testPolicy(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                auto alloc = map.get_allocator();
                REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                    bucketCount, testHasher(), testEqual(), testWeight(), testPolicy(), testAlloc())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map(cmpMap.cbegin(), cmpMap.cend(), capacity, bucketCount, testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map = std::make_unique<decltype(map)::element_type>(cmpMap.cbegin(), cmpMap.cend(),
                                                                                       GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount, testAlloc())), no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map(cmpMap.cbegin(), cmpMap.cend(), capacity, bucketCount, testHasher(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount, testHasher(), testAlloc())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + key_equal + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map(cmpMap.cbegin(), cmpMap.cend(), capacity,
                                                                                                      bucketCount, testHasher(), testEqual(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                auto equal = map.key_eq();
                REQUIRE(std::is_same_v<decltype(equal), testEqual>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                    bucketCount, testHasher(), testEqual(), testAlloc())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("Iterators + capacity + bucket_count + hasher + key_equal + weigher + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map(cmpMap.cbegin(), cmpMap.cend(), capacity,
                                                                                                      bucketCount, testHasher(), testEqual(), testWeight(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == capacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map =
                                           std::make_unique<decltype(map)::element_type>
                                                   (cmpMap.cbegin(), cmpMap.cend(), GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                    bucketCount, testHasher(), testEqual(), testWeight(), testAlloc())),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
    }
    SECTION("IList constructors")
    {
        size_t ilistCapacity = 3;
        auto bucketCount = GENERATE_ONE(1, 512);
        size_t count = 2;
        std::unordered_map<int, int> cmpMap{count};
        while(cmpMap.size() < count)
        {
            cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("IList + ilistCapacity")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1)))))), no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity, bucketCount);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}},
                                                                             GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount))), no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity, bucketCount, testHasher());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount, testHasher()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + key_equal")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity,
                                                                   bucketCount, testHasher(), testEqual());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                auto equal = map.key_eq();
                REQUIRE(std::is_same_v<decltype(equal), testEqual>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                      bucketCount, testHasher(), testEqual()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + key_equal + weigher")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity,
                                                                               bucketCount, testHasher(), testEqual(), testWeight());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                      bucketCount, testHasher(), testEqual(), testWeight()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + key_equal + weigher + policy")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity,
                                                                                           bucketCount, testHasher(), testEqual(),
                                                                                           testWeight(), testPolicy());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                      bucketCount, testHasher(), testEqual(), testWeight(), testPolicy()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + key_equal + weigher + policy + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity,
                                                                                                      bucketCount, testHasher(), testEqual(),
                                                                                                      testWeight(), testPolicy(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                auto alloc = map.get_allocator();
                REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                      bucketCount, testHasher(), testEqual(), testWeight(), testPolicy(), testAlloc()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity, bucketCount, testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}},
                                                                             GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount, testAlloc()))), no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity, bucketCount, testHasher(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))), bucketCount, testHasher(), testAlloc()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + key_equal + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity,
                                                                                                      bucketCount, testHasher(), testEqual(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                //May be larger as it rounds up to nearest prime number
                REQUIRE(map.bucket_count() >= bucketCount);
                auto hasher = map.hash_function();
                REQUIRE(std::is_same_v<decltype(hasher), testHasher>);
                auto equal = map.key_eq();
                REQUIRE(std::is_same_v<decltype(equal), testEqual>);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                      bucketCount, testHasher(), testEqual(), testAlloc()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
        SECTION("IList + ilistCapacity + bucket_count + hasher + key_equal + weigher + allocator")
        {
            SECTION("Can fit")
            {
                unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> map({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, ilistCapacity,
                                                                                                      bucketCount, testHasher(), testEqual(), testWeight(), testAlloc());
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
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
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                std::unique_ptr<unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc>> map;
                REQUIRE_THROWS_AS((map.reset(new decltype(map)::element_type
                                                     ({{cmpMap.begin()->first, cmpMap.begin()->second}, {(++cmpMap.begin())->first, (++cmpMap.begin())->second}}, GENERATE_REF(take(1, random(size_t(0), count - 1))),
                                                      bucketCount, testHasher(), testEqual(), testWeight(), testAlloc()))),
                                  no_space_error);
                REQUIRE(!map);
            }
        }
    }
    SECTION("Copy constructors")
    {
        unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> initMap(capacity);
        size_t count = GENERATE(take(1, random(size_t(2), capacity)));
        while(initMap.size() < count)
        {
            initMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("Preserve allocator")
        {
            decltype(initMap) map(initMap);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(map.load_factor() > 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            for(auto & i : map)
            {
                auto it = initMap.find(i.first);
                REQUIRE(it != initMap.cend());
                REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
            }
        }
        SECTION("Reset allocator")
        {
            decltype(initMap) map(initMap, testAlloc());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(map.load_factor() > 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
            for(auto & i : map)
            {
                auto it = initMap.find(i.first);
                REQUIRE(it != initMap.cend());
                REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
            }
        }
    }
    SECTION("Move constructors")
    {
        unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> initMap(capacity);
        size_t count = GENERATE(take(1, random(size_t(2), capacity)));
        while(initMap.size() < count)
        {
            initMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        decltype(initMap) mvMap(initMap);
        SECTION("Preserve allocator")
        {
            decltype(initMap) map(std::move(mvMap));
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(map.load_factor() > 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            for(auto & i : map)
            {
                auto it = initMap.find(i.first);
                REQUIRE(it != initMap.cend());
                REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
            }
        }
        SECTION("Reset allocator")
        {
            decltype(initMap) map(std::move(mvMap), testAlloc());
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(map.load_factor() > 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            auto alloc = map.get_allocator();
            REQUIRE(std::is_same_v<decltype(alloc), testAlloc>);
            for(auto & i : map)
            {
                auto it = initMap.find(i.first);
                REQUIRE(it != initMap.cend());
                REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
            }
        }
    }
    SECTION("Assignments")
    {
        unordered_map<int, int, testHasher, testEqual, testWeight, testPolicy, testAlloc> initMap(capacity);
        size_t count = GENERATE(take(1, random(size_t(2), capacity)));
        while(initMap.size() < count)
        {
            initMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("Copy")
        {
            decltype(initMap) map;
            REQUIRE(map.empty());
            map = initMap;
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(map.load_factor() > 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            for(auto & i : map)
            {
                auto it = initMap.find(i.first);
                REQUIRE(it != initMap.cend());
                REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
            }
        }
        SECTION("Move")
        {
            decltype(initMap) mvMap(initMap);
            decltype(initMap) map;
            REQUIRE(map.empty());
            map = std::move(mvMap);
            REQUIRE(!map.empty());
            REQUIRE(map.weight() == count);
            REQUIRE(map.capacity() == capacity);
            REQUIRE(std::distance(map.begin(), map.end()) == count);
            REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
            REQUIRE(map.size() == count);
            REQUIRE(map.load_factor() > 0);
            REQUIRE(map.max_size() > 0);
            REQUIRE(map.max_load_factor() > 0);
            REQUIRE(map.max_bucket_count() > 0);
            for(auto & i : map)
            {
                auto it = initMap.find(i.first);
                REQUIRE(it != initMap.cend());
                REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
            }
        }
        SECTION("IList")
        {
            size_t ilistCapacity = 3;
            size_t count = 2;
            std::unordered_map<int, int> cmpMap;
            cmpMap.reserve(count);
            while(cmpMap.size() < count)
            {
                cmpMap.emplace(GENERATE_ANY(int), GENERATE_ANY(int));
            }
            unordered_map<int, int> map(ilistCapacity);
            SECTION("Can fit")
            {
                map = {{cmpMap.begin()->first, cmpMap.begin()->second},
                       {(++cmpMap.begin())->first, (++cmpMap.begin())->second}};
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
            }
            SECTION("Can't fit")
            {
                map = {{cmpMap.begin()->first, cmpMap.begin()->second},
                       {(++cmpMap.begin())->first, (++cmpMap.begin())->second}};
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.load_factor() > 0);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                for(auto & i : map)
                {
                    auto it = cmpMap.find(i.first);
                    REQUIRE(it != cmpMap.cend());
                    REQUIRE(it->first == i.first);
                    REQUIRE(it->second == i.second);
                }
                while(cmpMap.size() < ilistCapacity + 1)
                {
                    cmpMap.emplace(GENERATE_ANY(int), GENERATE_ANY(int));
                }
                REQUIRE_THROWS_AS((map = {{cmpMap.begin()->first, cmpMap.begin()->second},
                                          {(++cmpMap.begin())->first, (++cmpMap.begin())->second},
                                          {(++++cmpMap.begin())->first, (++++cmpMap.begin())->second},
                                          {(++++++cmpMap.begin())->first, (++++++cmpMap.begin())->second}}), no_space_error);
                REQUIRE(!map.empty());
                REQUIRE(map.weight() == count);
                REQUIRE(map.capacity() == ilistCapacity);
                REQUIRE(std::distance(map.begin(), map.end()) == count);
                REQUIRE(std::distance(map.replacement_begin(), map.replacement_end()) == count);
                REQUIRE(map.size() == count);
                REQUIRE(map.max_size() > 0);
                REQUIRE(map.max_load_factor() > 0);
                REQUIRE(map.max_bucket_count() > 0);
                for(auto & i : map)
                {
                    REQUIRE(cmpMap.find(i.first) != cmpMap.end());
                }
            }
        }
    }
    SECTION("Swap")
    {
        auto capacity1 = GENERATE(take(1, random(capacity/2, (capacity*3)/2)));
        auto capacity2 = GENERATE(take(1, random(capacity/2, (capacity*3)/2)));
        std::unordered_map<int, int> cmpMap1;
        std::unordered_map<int, int> cmpMap2;
        size_t count1 = GENERATE_REF(take(1, random(size_t(1), capacity1)));
        size_t count2 = GENERATE_REF(take(1, random(size_t(1), capacity2)));
        while(cmpMap1.size() < count1)
        {
            cmpMap1.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        while(cmpMap2.size() < count2)
        {
            cmpMap2.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        unordered_map<int, int> map1(capacity1);
        unordered_map<int, int> map2(capacity2);
        map1.insert(cmpMap1.begin(), cmpMap1.end());
        map2.insert(cmpMap2.begin(), cmpMap2.end());
        SECTION("With class method")
        {
            map1.swap(map2);
            REQUIRE(!map1.empty());
            REQUIRE(map1.weight() == count2);
            REQUIRE(map1.capacity() == capacity2);
            REQUIRE(std::distance(map1.begin(), map1.end()) == count2);
            REQUIRE(std::distance(map1.replacement_begin(), map1.replacement_end()) == count2);
            REQUIRE(map1.size() == count2);
            for (auto &i : map1)
            {
                auto it = cmpMap2.find(i.first);
                REQUIRE(it != cmpMap2.cend());
                REQUIRE(it->second == i.second);

            }
            REQUIRE(!map2.empty());
            REQUIRE(map2.weight() == count1);
            REQUIRE(map2.capacity() == capacity1);
            REQUIRE(std::distance(map2.begin(), map2.end()) == count1);
            REQUIRE(std::distance(map2.replacement_begin(), map2.replacement_end()) == count1);
            REQUIRE(map2.size() == count1);
            for (auto &i : map2)
            {
                auto it = cmpMap1.find(i.first);
                REQUIRE(it != cmpMap1.cend());
                REQUIRE(it->second == i.second);

            }
        }
        SECTION("With external method")
        {
            swap(map1, map2);
            REQUIRE(!map1.empty());
            REQUIRE(map1.weight() == count2);
            REQUIRE(map1.capacity() == capacity2);
            REQUIRE(std::distance(map1.begin(), map1.end()) == count2);
            REQUIRE(std::distance(map1.replacement_begin(), map1.replacement_end()) == count2);
            REQUIRE(map1.size() == count2);
            for (auto &i : map1)
            {
                auto it = cmpMap2.find(i.first);
                REQUIRE(it != cmpMap2.cend());
                REQUIRE(it->second == i.second);

            }
            REQUIRE(!map2.empty());
            REQUIRE(map2.weight() == count1);
            REQUIRE(map2.capacity() == capacity1);
            REQUIRE(std::distance(map2.begin(), map2.end()) == count1);
            REQUIRE(std::distance(map2.replacement_begin(), map2.replacement_end()) == count1);
            REQUIRE(map2.size() == count1);
            for (auto &i : map2)
            {
                auto it = cmpMap1.find(i.first);
                REQUIRE(it != cmpMap1.cend());
                REQUIRE(it->second == i.second);

            }
        }
    }
    SECTION("Compare")
    {
        std::unordered_map<int, int> cmpMap;
        size_t count = GENERATE(take(1, random(size_t(2), capacity)));
        while(cmpMap.size() < count)
        {
            cmpMap.insert({GENERATE_ANY(int), GENERATE_ANY(int)});
        }
        SECTION("Equal")
        {
            unordered_map<int, int> map1(cmpMap.cbegin(), cmpMap.cend(), capacity);
            unordered_map<int, int> map2(cmpMap.cbegin(), cmpMap.cend(), capacity);
            REQUIRE(map1 == map2);
            REQUIRE_FALSE(map1 != map2);
        }
        SECTION("Not equal")
        {
            unordered_map<int, int> map1(cmpMap.cbegin(), cmpMap.cend(), capacity);
            unordered_map<int, int> map2(cmpMap.cbegin(), cmpMap.cend(), capacity);
            map1.erase(map1.begin());
            REQUIRE(map1 != map2);
            REQUIRE_FALSE(map1 == map2);
        }
    }
}
