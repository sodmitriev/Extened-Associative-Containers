#include <cache_manager.h>
#include <forward_list>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define GENERATE_ONE(a, b) GENERATE(take(1, random(a, b)))
#define GENERATE_ANY(type) GENERATE(take(1, random(std::numeric_limits<type>::min(), std::numeric_limits<type>::max())))

class CustomSize : public cache_manager::weight<int>
{
public:
    int check;
    CustomSize(int ch) : check(ch) {}
};

class CustomPolicy : public cache_manager::policy::fifo<int>
{
public:
    int check;
    CustomPolicy(int ch) : check(ch) {}
};

struct CustomStruct
{
    size_t val;
    CustomStruct() = default;
    explicit CustomStruct(size_t value) {val = value;}
};

TEMPLATE_TEST_CASE_SIG("Cache manager test", "[extended containers][limited containers][cache manager]",
        ((size_t capacity), capacity), 1024)
{
    SECTION("Construct")
    {
        SECTION("Default")
        {
            cache_manager::cache_manager<int> manager;
            REQUIRE(manager.weight() == 0);
            REQUIRE(manager.capacity() == 0);
            REQUIRE(manager.cbegin() == manager.cend());
        }
        SECTION("Set capacity")
        {
            auto val = GENERATE_ANY(size_t);
            cache_manager::cache_manager<int> manager(val);
            REQUIRE(manager.weight() == 0);
            REQUIRE(manager.capacity() == val);
            REQUIRE(manager.cbegin() == manager.cend());
        }
        SECTION("Move")
        {
            auto val = GENERATE_ANY(size_t);
            cache_manager::cache_manager<int> manager(val);
            REQUIRE(manager.weight() == 0);
            REQUIRE(manager.capacity() == val);
            REQUIRE(manager.cbegin() == manager.cend());
            cache_manager::cache_manager<int> manager1(std::move(manager));
            REQUIRE(manager1.weight() == 0);
            REQUIRE(manager1.capacity() == val);
            REQUIRE(manager.cbegin() == manager.cend());
        }
        SECTION("Move with values")
        {
            cache_manager::cache_manager<int> manager(capacity);
            size_t count = GENERATE_ONE(size_t(0), capacity - 1);
            std::vector<cache_manager::stored_node<int>> vals;
            vals.reserve(count);
            for(size_t i = 0; i < count; ++i)
            {
                vals.push_back({GENERATE_ANY(int), {}});
            }
            for(auto & i : vals)
            {
                manager.insert(&i);
            }
            REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
            auto manager1(std::move(manager));
            std::set<const int*> set;
            for(auto & i : vals)
            {
                set.insert(&i.first);
            }
            for(auto & i : manager1)
            {
                REQUIRE(set.erase(&i) == 1);
            }
            REQUIRE(set.empty());
        }
    }
    SECTION("Insert")
    {
        cache_manager::cache_manager<int> manager(capacity);
        size_t count = GENERATE_ONE(size_t(0), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        for(auto & i : vals)
        {
            manager.insert(&i);
        }
        REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
    }
    SECTION("Find")
    {
        cache_manager::cache_manager<int> manager(capacity);
        size_t count = GENERATE_ONE(size_t(0), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        for(auto & i : vals)
        {
            manager.insert(&i);
        }
        REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
        SECTION("Not const")
        {
            for(auto & i : vals)
            {
                auto it = cache_manager::cache_manager<int >::toIter(&i);
                REQUIRE(it != manager.cend());
                REQUIRE(&*it == &i.first);
            }
        }
        SECTION("Const")
        {
            const decltype(manager)& manager1 = manager;
            for(auto & i : vals)
            {
                auto it = decltype(manager)::toIter(&i);
                REQUIRE(it != manager.cend());
                REQUIRE(&*it == &i.first);
                REQUIRE(manager1.cbegin() != manager1.cend());
            }
        }
    }
    SECTION("Erase")
    {
        cache_manager::cache_manager<int> manager(capacity);
        size_t count = GENERATE_ONE(size_t(1), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        for(auto & i : vals)
        {
            manager.insert(&i);
        }
        REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
        auto pos = GENERATE_ANY(size_t)%vals.size();
        auto ptr = &vals.at(pos);
        SECTION("By value")
        {
            auto it = decltype(manager)::toIter(ptr);
            REQUIRE(it != manager.cend());
            REQUIRE_NOTHROW(manager.erase(ptr));
        }
        SECTION("By iterator")
        {
            cache_manager::replacement_iterator<int> it = decltype(manager)::toIter(ptr);
            REQUIRE(it != manager.cend());
            auto nextIt = it;
            ++nextIt;
            auto prevIt = it;
            --prevIt;
            REQUIRE(manager.erase(it) == nextIt);
            REQUIRE(++decltype(prevIt)(prevIt) == nextIt);
            REQUIRE(--decltype(nextIt)(nextIt) == prevIt);
        }
        SECTION("By const iterator")
        {
            cache_manager::const_replacement_iterator<int> it = decltype(manager)::toIter(ptr);
            REQUIRE(it != manager.cend());
            auto nextIt = it;
            ++nextIt;
            auto prevIt = it;
            --prevIt;
            REQUIRE(manager.erase(it) == nextIt);
            REQUIRE(++decltype(prevIt)(prevIt) == nextIt);
            REQUIRE(--decltype(nextIt)(nextIt) == prevIt);
        }
    }
    SECTION("replacement_iterator getters")
    {
        //Refer to IteratorTest for more complex test cases
        cache_manager::cache_manager<int> manager(capacity);
        size_t count = GENERATE_ONE(size_t(0), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        for(auto & i : vals)
        {
            manager.insert(&i);
        }
        REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
        SECTION("Begin")
        {
            auto it1 = manager.begin();
            auto it2 = manager.cbegin();
            const decltype(manager)& manager1 = manager;
            auto it3 = manager1.begin();
            REQUIRE(it1 == it2);
            REQUIRE(it1 == it3);
            REQUIRE(it2 == it3);
        }
        SECTION("End")
        {
            auto it1 = manager.end();
            auto it2 = manager.cend();
            const decltype(manager)& manager1 = manager;
            auto it3 = manager1.end();
            REQUIRE(it1 == it2);
            REQUIRE(it1 == it3);
            REQUIRE(it2 == it3);
        }
    }
    SECTION("Clear")
    {
        cache_manager::cache_manager<int> manager(capacity);
        size_t count = GENERATE_ONE(size_t(0), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        for(auto & i : vals)
        {
            manager.insert(&i);
        }
        REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
        REQUIRE_NOTHROW(manager.clear());
        REQUIRE(manager.weight() == 0);
        REQUIRE(manager.capacity() == capacity);
        REQUIRE(manager.cbegin() == manager.cend());
    }
    SECTION("Get next")
    {
        cache_manager::cache_manager<int> manager(capacity);
        size_t count = GENERATE_ONE(size_t(1), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        SECTION("Without hint")
        {
            SECTION("From empty")
            {
                REQUIRE(manager.next() == manager.cend());
            }
            SECTION("From filled")
            {
                for(auto & i : vals)
                {
                    manager.insert(&i);
                }
                REQUIRE(manager.next() == manager.cbegin());
            }
        }
        SECTION("With hint")
        {
            SECTION("Cend hint")
            {
                REQUIRE(manager.next(manager.cend()) == manager.cend());
            }
            SECTION("From filled")
            {
                for(auto & i : vals)
                {
                    manager.insert(&i);
                }
                auto pos = GENERATE_ANY(size_t)%vals.size();
                auto ptr = &vals.at(pos);
                auto it = manager.toIter(ptr);
                REQUIRE(manager.next(it) == it);
            }
        }

    }
    SECTION("Access")
    {
        cache_manager::cache_manager<int, cache_manager::weight<int>, cache_manager::policy::lru<int>> manager(capacity);
        size_t count = GENERATE_ONE(size_t(1), capacity - 1);
        std::vector<cache_manager::stored_node<int>> vals;
        vals.reserve(count);
        for(size_t i = 0; i < count; ++i)
        {
            vals.push_back({GENERATE_ANY(int), {}});
        }
        auto pos = GENERATE_ANY(size_t)%vals.size();
        auto ptr = &vals.at(pos);
        for(auto & i : vals)
        {
            manager.insert(&i);
        }
        SECTION("By pointer")
        {
            REQUIRE_NOTHROW(manager.access(ptr));
            REQUIRE(&*--manager.cend() == &ptr->first);
        }
        SECTION("By iterator")
        {
            auto it = manager.toIter(ptr);
            REQUIRE(it != manager.cend());
            REQUIRE_NOTHROW(manager.access(it));
            REQUIRE(&*--manager.cend() == &ptr->first);
        }
    }
    SECTION("Custom template types")
    {
        CustomSize sizer(GENERATE_ANY(int));
        CustomPolicy policy(GENERATE_ANY(int));
        cache_manager::cache_manager<int, CustomSize, CustomPolicy> manager(capacity, sizer, policy);
        REQUIRE(sizer.check == manager.get_weigher().check);
        REQUIRE(policy.check == manager.get_policy().check);
    }
    SECTION("Custom size")
    {
        cache_manager::cache_manager<CustomStruct, std::function<size_t(const CustomStruct&)>,
                cache_manager::policy::lru<CustomStruct>>
            manager(capacity, [](const CustomStruct& val) { return val.val; });
        SECTION("Calculate size")
        {
            CustomStruct val {GENERATE_ANY(size_t)};
            REQUIRE(manager.calculate_weight(val) == val.val);
        }
        SECTION("Check can fit")
        {
            std::forward_list<cache_manager::stored_node<CustomStruct>> vals;
            while(manager.capacity() - manager.weight() > capacity/2)
            {
                size_t val = GENERATE_ANY(size_t)%((manager.capacity() - manager.weight())/2);
                vals.push_front({CustomStruct{val}, {}});
                manager.insert(&vals.front());
            }
            SECTION("With size")
            {
                SECTION("Can fit")
                {
                    size_t val = GENERATE_ANY(size_t)%(manager.capacity() - manager.weight());
                    REQUIRE(manager.can_fit(val));
                }
                SECTION("Can't fit")
                {
                    size_t val = GENERATE_ANY(size_t)%1024 + manager.capacity() - manager.weight() + 1;
                    REQUIRE(!manager.can_fit(val));
                }
            }
            SECTION("With value")
            {
                SECTION("Can fit")
                {
                    size_t val = GENERATE_ANY(size_t)%(manager.capacity() - manager.weight());
                    REQUIRE(manager.can_fit(CustomStruct{val}));
                }
                SECTION("Can't fit")
                {
                    size_t val = GENERATE_ANY(size_t)%1024 + manager.capacity() - manager.weight() + 1;
                    REQUIRE(!manager.can_fit(CustomStruct{val}));
                }
            }

        }
    }
    SECTION("Move assignment")
    {
        SECTION("Without values")
        {
            auto val = GENERATE_ANY(size_t);
            cache_manager::cache_manager<int> manager(val);
            REQUIRE(manager.weight() == 0);
            REQUIRE(manager.capacity() == val);
            REQUIRE(manager.cbegin() == manager.cend());
            cache_manager::cache_manager<int> manager1;
            manager1 = std::move(manager);
            REQUIRE(manager1.weight() == 0);
            REQUIRE(manager1.capacity() == val);
            REQUIRE(manager.cbegin() == manager.cend());
        }
        SECTION("With values")
        {
            cache_manager::cache_manager<int> manager(capacity);
            size_t count = GENERATE_ONE(size_t(0), capacity - 1);
            std::vector<cache_manager::stored_node<int>> vals;
            vals.reserve(count);
            for(size_t i = 0; i < count; ++i)
            {
                vals.push_back({GENERATE_ANY(int), {}});
            }
            for(auto & i : vals)
            {
                manager.insert(&i);
            }
            REQUIRE(std::distance(manager.begin(), manager.end()) == vals.size());
            decltype(manager) manager1;
            manager1 = std::move(manager);
            std::set<const int*> set;
            for(auto & i : vals)
            {
                set.insert(&i.first);
            }
            for(auto & i : manager1)
            {
                REQUIRE(set.erase(&i) == 1);
            }
            REQUIRE(set.empty());
        }

    }
}
