#include <cache_manager.h>
#include <array>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif // defined(__clang__)

#define GENERATE_ONE(a, b) GENERATE(take(1, random(a, b)))
#define GENERATE_ANY(type) GENERATE(take(1, random(std::numeric_limits<type>::min(), std::numeric_limits<type>::max())))

std::array<cache_manager::replacement_iterator<int>, 6> getIters(cache_manager::replacement_iterator<int> begin, size_t first_off, size_t first_len, size_t second_off, size_t second_len)
{
    auto before_first = begin;
    for(size_t i = 0 ; i < first_off; ++i, ++before_first);
    auto first = before_first;
    ++first;
    auto after_first = first;
    for(size_t i = 0 ; i < first_len; ++i, ++after_first);
    auto before_second = after_first;
    for(size_t i = 0 ; i < second_off; ++i, ++before_second);
    auto second = before_second;
    ++second;
    auto after_second = second;
    for(size_t i = 0 ; i < second_len; ++i, ++after_second);
    return {before_first, first, after_first, before_second, second, after_second};
}

TEMPLATE_TEST_CASE_SIG("Cache manager iterator test", "[extended containers][limited containers][cache manager][replacement iterator]",
        ((size_t size), size), 1024)
{
    std::vector<cache_manager::stored_node<int>> vals;
    vals.reserve(size);
    for(size_t i = 0; i < size; ++i)
    {
        vals.push_back({GENERATE_ANY(int), {}});
    }

    cache_manager::cache_manager<int> manager(std::numeric_limits<size_t>::max());
    for(auto & i : vals)
    {
        manager.insert(&i);
    }

    SECTION("replacement_iterator traits")
    {
        REQUIRE(std::is_same_v<typename std::iterator_traits<cache_manager::replacement_iterator<int>>::difference_type, ptrdiff_t>);
        REQUIRE(std::is_same_v<typename std::iterator_traits<cache_manager::replacement_iterator<int>>::value_type , const int>);
        REQUIRE(std::is_same_v<typename std::iterator_traits<cache_manager::replacement_iterator<int>>::pointer , const int*>);
        REQUIRE(std::is_same_v<typename std::iterator_traits<cache_manager::replacement_iterator<int>>::reference, const int&>);
        REQUIRE(std::is_same_v<typename std::iterator_traits<cache_manager::replacement_iterator<int>>::iterator_category, std::bidirectional_iterator_tag>);
    }

    SECTION("Basic operations")
    {
        SECTION("Non const iterator")
        {
            typedef cache_manager::replacement_iterator<int> iter_t;
            SECTION("Default constructor")
            {
                std::unique_ptr<iter_t> ptr;
                REQUIRE_NOTHROW(ptr = std::make_unique<iter_t>());
            }
            SECTION("Copy constructor")
            {
                iter_t it1 = manager.begin();
                iter_t it2(it1);
                REQUIRE(it1 == it2);
            }
            SECTION("Assignment")
            {
                iter_t it1 = manager.begin();
                iter_t it2(it1);
                it1 = manager.end();
                REQUIRE(it1 != it2);
                it2 = it1;
                REQUIRE(it1 == it2);
            }
            SECTION("Swap")
            {
                iter_t it1 = manager.begin();
                iter_t it2 = manager.end();
                it1.swap(it2);
                REQUIRE(it1 == manager.end());
                REQUIRE(it2 == manager.begin());
                std::swap(it1, it2);
                REQUIRE(it1 == manager.begin());
                REQUIRE(it2 == manager.end());
                cache_manager::swap(it1, it2);
                REQUIRE(it1 == manager.end());
                REQUIRE(it2 == manager.begin());

            }
            SECTION("Operator *")
            {
                cache_manager::stored_node<int> val {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
                cache_manager::replacement_iterator<int> it = manager.insert(&val);
                REQUIRE(*it == val.first);
            }
            SECTION("Operator ->")
            {
                cache_manager::stored_node<int> val {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
                cache_manager::replacement_iterator<int> it = manager.insert(&val);
                REQUIRE(it.operator->() == &val.first);
            }
            SECTION("Prefix increment")
            {
                iter_t prev_it = manager.begin();
                iter_t it = manager.begin();
                size_t count = 0;
                while(it != manager.end())
                {
                    ++count;
                    ++it;
                    REQUIRE(it != prev_it);
                    ++prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Postfix increment")
            {
                iter_t prev_it = manager.begin();
                iter_t it = manager.begin();
                size_t count = 0;
                while(it != manager.end())
                {
                    ++count;
                    prev_it = it++;
                    REQUIRE(it != prev_it);
                    ++prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Prefix decrement")
            {
                iter_t prev_it = manager.end();
                iter_t it = manager.end();
                size_t count = 0;
                while(it != manager.begin())
                {
                    ++count;
                    --it;
                    REQUIRE(it != prev_it);
                    --prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Postfix decrement")
            {
                iter_t prev_it = manager.end();
                iter_t it = manager.end();
                size_t count = 0;
                while(it != manager.begin())
                {
                    ++count;
                    prev_it = it--;
                    REQUIRE(it != prev_it);
                    --prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Compare operators")
            {
                iter_t it1 = manager.begin();
                iter_t it2 = manager.begin();
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                --it1;
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                ++it2;
                REQUIRE(it1 == it2);
            }
        }
        SECTION("Const iterator")
        {
            typedef cache_manager::const_replacement_iterator<int> iter_t;
            SECTION("Default constructor")
            {
                std::unique_ptr<iter_t> ptr;
                REQUIRE_NOTHROW(ptr = std::make_unique<iter_t>());
            }
            SECTION("Copy constructor")
            {
                iter_t it1 = manager.cbegin();
                iter_t it2(it1);
                REQUIRE(it1 == it2);
            }
            SECTION("Assignment")
            {
                iter_t it1 = manager.cbegin();
                iter_t it2(it1);
                it1 = manager.cend();
                REQUIRE(it1 != it2);
                it2 = it1;
                REQUIRE(it1 == it2);
            }
            SECTION("Swap")
            {
                iter_t it1 = manager.cbegin();
                iter_t it2 = manager.cend();
                it1.swap(it2);
                REQUIRE(it1 == manager.cend());
                REQUIRE(it2 == manager.cbegin());
                std::swap(it1, it2);
                REQUIRE(it1 == manager.cbegin());
                REQUIRE(it2 == manager.cend());
            }
            SECTION("Operator *")
            {
                cache_manager::stored_node<int> val {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
                auto it = manager.insert(&val);
                REQUIRE(*it == val.first);
            }
            SECTION("Operator ->")
            {
                cache_manager::stored_node<int> val {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
                auto it = manager.insert(&val);
                REQUIRE(it.operator->() == &val.first);
            }
            SECTION("Prefix increment")
            {
                iter_t prev_it = manager.cbegin();
                iter_t it = manager.cbegin();
                size_t count = 0;
                while(it != manager.cend())
                {
                    ++count;
                    ++it;
                    REQUIRE(it != prev_it);
                    ++prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Postfix increment")
            {
                iter_t prev_it = manager.cbegin();
                iter_t it = manager.cbegin();
                size_t count = 0;
                while(it != manager.cend())
                {
                    ++count;
                    prev_it = it++;
                    REQUIRE(it != prev_it);
                    ++prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Prefix decrement")
            {
                iter_t prev_it = manager.cend();
                iter_t it = manager.cend();
                size_t count = 0;
                while(it != manager.cbegin())
                {
                    ++count;
                    --it;
                    REQUIRE(it != prev_it);
                    --prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Postfix decrement")
            {
                iter_t prev_it = manager.cend();
                iter_t it = manager.cend();
                size_t count = 0;
                while(it != manager.cbegin())
                {
                    ++count;
                    prev_it = it--;
                    REQUIRE(it != prev_it);
                    --prev_it;
                    REQUIRE(it == prev_it);
                }
                REQUIRE_NOTHROW(count = size);
            }
            SECTION("Compare operators")
            {
                iter_t it1 = manager.cbegin();
                iter_t it2 = manager.cbegin();
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                --it1;
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                ++it2;
                REQUIRE(it1 == it2);
            }
        }
        SECTION("Const - non const operations")
        {
            SECTION("Convert non const to const")
            {
                cache_manager::replacement_iterator<int> it1 = manager.begin();
                std::unique_ptr<cache_manager::const_replacement_iterator<int>> ptr;
                REQUIRE_NOTHROW(ptr = std::make_unique<cache_manager::const_replacement_iterator<int>>(it1));
            }
            SECTION("Compare const - non const")
            {
                cache_manager::const_replacement_iterator<int> it1 = manager.begin();
                cache_manager::replacement_iterator<int> it2 = manager.begin();
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                --it1;
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                ++it2;
                REQUIRE(it1 == it2);
            }
            SECTION("Compare non const - const")
            {
                cache_manager::replacement_iterator<int> it1 = manager.begin();
                cache_manager::const_replacement_iterator<int> it2 = manager.begin();
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                --it1;
                REQUIRE(it1 == it2);
                ++it1;
                REQUIRE(it1 != it2);
                ++it2;
                REQUIRE(it1 == it2);
            }
        }
    }

    SECTION("Extended operations")
    {
        SECTION("Move")
        {
            SECTION("Move one element")
            {
                auto [before_first, first, after_first, before_second, second, after_second] =
                        getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1,
                                                  GENERATE_ONE(size_t(0), size/3), 1);
                SECTION("Before destination")
                {
                    cache_manager::iterator_operations::move(first, after_first, second);
                    auto it = before_first;
                    ++it;
                    REQUIRE(it == after_first);
                    it = before_second;
                    ++it;
                    REQUIRE(it == first);
                    ++it;
                    REQUIRE(it == second);
                    ++it;
                    REQUIRE(it == after_second);
                }
                SECTION("After destination")
                {
                    cache_manager::iterator_operations::move(second, after_second, first);
                    auto it = before_second;
                    ++it;
                    REQUIRE(it == after_second);
                    it = before_first;
                    ++it;
                    REQUIRE(it == second);
                    ++it;
                    REQUIRE(it == first);
                    ++it;
                    REQUIRE(it == after_first);
                }
            }
            SECTION("Move many elements")
            {
                auto first_len = GENERATE_ONE(size_t(2), size/6);
                auto second_len = GENERATE_ONE(size_t(2), size/6);
                auto [before_first, first, after_first, before_second, second, after_second] =
                getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), first_len,
                                          GENERATE_ONE(size_t(0), size/6), second_len);
                SECTION("Before destination")
                {
                    cache_manager::iterator_operations::move(first, after_first, second);
                    auto it1 = before_first;
                    auto it2 = first;
                    ++it1;
                    REQUIRE(it1 == after_first);
                    it1 = before_second;
                    ++it1;
                    for(size_t i = 0 ; i < first_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == second);
                    for(size_t i = 0 ; i < second_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == after_second);
                }
                SECTION("After destination")
                {
                    cache_manager::iterator_operations::move(second, after_second, first);
                    auto it1 = before_second;
                    auto it2 = second;
                    ++it1;
                    REQUIRE(it1 == after_second);
                    it1 = before_first;
                    ++it1;
                    for(size_t i = 0 ; i < second_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == first);
                    for(size_t i = 0 ; i < first_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == after_first);
                }
            }
        }
        SECTION("Iter swap")
        {
            SECTION("Randomly located")
            {
                auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1,
                                              GENERATE_ONE(size_t(0), size/3), 1);
                cache_manager::iterator_operations::iter_swap(first, second);
                auto it = before_first;
                ++it;
                REQUIRE(it == second);
                ++it;
                REQUIRE(it == after_first);
                it = before_second;
                ++it;
                REQUIRE(it == first);
                ++it;
                REQUIRE(it == after_second);
            }
            SECTION("Adjacent first before second")
            {
                auto [before_first, first, after_first, before_second, second, after_second] =
                getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1, 0, 1);
                --second;
                --after_second;
                cache_manager::iterator_operations::iter_swap(first, second);
                auto it = before_first;
                ++it;
                REQUIRE(it == second);
                ++it;
                REQUIRE(it == first);
                ++it;
                REQUIRE(it == after_second);
            }
            SECTION("Adjacent second before first")
            {
                auto [before_first, first, after_first, before_second, second, after_second] =
                getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1, 0, 1);
                --second;
                --after_second;
                cache_manager::iterator_operations::iter_swap(second, first);
                auto it = before_first;
                ++it;
                REQUIRE(it == second);
                ++it;
                REQUIRE(it == first);
                ++it;
                REQUIRE(it == after_second);
            }
        }
        SECTION("Swap ranges")
        {
            SECTION("Single element")
            {
                SECTION("Randomly located")
                {
                    auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1,
                                              GENERATE_ONE(size_t(0), size/3), 1);
                    cache_manager::iterator_operations::swap_ranges(first, after_first, second, after_second);
                    auto it = before_first;
                    ++it;
                    REQUIRE(it == second);
                    ++it;
                    REQUIRE(it == after_first);
                    it = before_second;
                    ++it;
                    REQUIRE(it == first);
                    ++it;
                    REQUIRE(it == after_second);
                }
                SECTION("Adjacent first before second")
                {
                    auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1, 0, 1);
                    --second;
                    --after_second;
                    cache_manager::iterator_operations::swap_ranges(first, after_first, second, after_second);
                    auto it = before_first;
                    ++it;
                    REQUIRE(it == second);
                    ++it;
                    REQUIRE(it == first);
                    ++it;
                    REQUIRE(it == after_second);
                }
                SECTION("Adjacent second before first")
                {
                    auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/3), 1, 0, 1);
                    --second;
                    --after_second;
                    cache_manager::iterator_operations::swap_ranges(second, after_second, first, after_first);
                    auto it = before_first;
                    ++it;
                    REQUIRE(it == second);
                    ++it;
                    REQUIRE(it == first);
                    ++it;
                    REQUIRE(it == after_second);
                }
            }
            SECTION("Range")
            {
                auto first_len = GENERATE_ONE(size_t(2), size/6);
                auto second_len = GENERATE_ONE(size_t(2), size/6);
                SECTION("Randomly located")
                {
                    auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), first_len,
                             GENERATE_ONE(size_t(0), size/6), second_len);
                    cache_manager::iterator_operations::swap_ranges(first, after_first, second, after_second);
                    auto it1 = before_first;
                    ++it1;
                    auto it2 = second;
                    for(size_t i = 0; i < second_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == after_first);
                    it1 = before_second;
                    ++it1;
                    it2 = first;
                    for(size_t i = 0; i < first_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == after_second);
                }
                SECTION("Adjacent first before second")
                {
                    auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), first_len, 0, second_len);
                    --second;
                    --after_second;
                    cache_manager::iterator_operations::swap_ranges(first, after_first, second, after_second);
                    auto it1 = before_first;
                    ++it1;
                    auto it2 = second;
                    for(size_t i = 0; i < second_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == first);
                    for(size_t i = 0; i < first_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == after_second);
                }
                SECTION("Adjacent second before first")
                {
                    auto [before_first, first, after_first, before_second, second, after_second] =
                    getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), first_len, 0, second_len);
                    --second;
                    --after_second;
                    cache_manager::iterator_operations::swap_ranges(second, after_second, first, after_first);
                    auto it1 = before_first;
                    ++it1;
                    auto it2 = second;
                    for(size_t i = 0; i < second_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == first);
                    for(size_t i = 0; i < first_len; ++i)
                    {
                        REQUIRE(it1 == it2);
                        ++it1;
                        ++it2;
                    }
                    REQUIRE(it1 == after_second);
                }
            }
        }
        SECTION("Reverse")
        {
            SECTION("One element")
            {
                auto [before_first, first, after_first, before_second, second, after_second] =
                getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), 1, 0, 0);
                cache_manager::iterator_operations::reverse(first, after_first);
                auto it = before_first;
                ++it;
                REQUIRE(it == first);
                ++it;
                REQUIRE(it == after_first);
            }
            SECTION("Multiple elments")
            {
                auto first_len = GENERATE_ONE(size_t(2), size/6);
                auto [before_first, first, after_first, before_second, second, after_second] =
                getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), first_len, 0, 0);
                std::vector<cache_manager::replacement_iterator<int>> initOrder;
                initOrder.reserve(first_len);
                for(auto it = first; it != after_first; ++it)
                {
                    initOrder.push_back(it);
                }
                cache_manager::iterator_operations::reverse(first, after_first);
                auto it = before_first;
                ++it;
                auto rit = initOrder.rbegin();
                for(size_t i = 0; i < first_len; ++i)
                {
                    REQUIRE(it == *rit);
                    ++it;
                    ++rit;
                }
                REQUIRE(it == after_first);
            }

        }
        SECTION("Rotate")
        {
            auto first_len = GENERATE_ONE(size_t(2), size/6);
            auto second_len = GENERATE_ONE(size_t(2), size/6);
            auto [before_first, first, after_first, before_second, second, after_second] =
            getIters(manager.begin(), GENERATE_ONE(size_t(0), size/6), first_len, 0, second_len);
            cache_manager::iterator_operations::rotate(first, second, after_second);
            auto it1 = before_first;
            ++it1;
            auto it2 = second;
            for(size_t i = 0; i < second_len; ++i)
            {
                REQUIRE(it1 == it2);
                ++it1;
                ++it2;
            }
            REQUIRE(it1 == first);
            for(size_t i = 0; i < first_len; ++i)
            {
                REQUIRE(it1 == it2);
                ++it1;
                ++it2;
            }
            REQUIRE(it1 == after_first);
            it1 = before_second;
            ++it1;
            REQUIRE(it1 == after_second);
        }
    }

}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)
