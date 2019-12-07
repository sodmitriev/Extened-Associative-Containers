#include <cache_manager.h>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define GENERATE_ONE(a, b) GENERATE(take(1, random(a, b)))
#define GENERATE_ANY(type) GENERATE(take(1, random(std::numeric_limits<type>::min(), std::numeric_limits<type>::max())))

template <class Policy>
void fill(cache_manager::cache_manager<int, cache_manager::weight<int>, Policy>& manager, std::vector<cache_manager::stored_node<int>>& src)
{
    for(auto &i : src)
    {
        manager.insert(&i);
    }
}

TEMPLATE_TEST_CASE_SIG("Cache manager policy test", "[extended containers][limited containers][cache manager][cache policy]",
                       ((size_t size), size), 1024)
{
    std::vector<cache_manager::stored_node<int>> vals;
    vals.reserve(size);
    for(size_t i = 0; i < size; ++i)
    {
        vals.push_back({GENERATE_ANY(int), {}});
    }
    SECTION("FIFO")
    {
        cache_manager::cache_manager<int, cache_manager::weight<int>, cache_manager::policy::fifo<int>> cache(2048);
        fill(cache, vals);
        SECTION("Erase")
        {
            REQUIRE(cache.next() == cache.cbegin());
        }
        SECTION("Insert")
        {
            cache_manager::stored_node<int> v {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
            auto it = cache.insert(&v);
            auto last = cache.cend();
            --last;
            REQUIRE(it == last);
            REQUIRE(&(*it) == &v.first);
        }
        SECTION("Access")
        {
            SECTION("Access any")
            {
                auto pos = GENERATE_ONE(size_t(0), size - 1);
                auto ptr = &vals.at(pos);
                auto it = cache.toIter(ptr);
                REQUIRE(it != cache.cend());
                cache.access(it);
                auto it1 = cache.cbegin();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    ++it1;
                }
            }
            SECTION("Access first")
            {
                cache.access(cache.begin());
                auto it1 = cache.cbegin();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    ++it1;
                }
            }
            SECTION("Access last")
            {
                cache.access(--cache.end());
                auto it1 = cache.cbegin();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    ++it1;
                }
            }
        }
    }
    SECTION("LIFO")
    {
        cache_manager::cache_manager<int, cache_manager::weight<int>, cache_manager::policy::lifo<int>> cache(2048);
        fill(cache, vals);
        SECTION("Erase")
        {
            REQUIRE(cache.next() == cache.cbegin());
        }
        SECTION("Insert")
        {
            cache_manager::stored_node<int> v {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
            auto it = cache.insert(&v);
            REQUIRE(it == cache.cbegin());
            REQUIRE(&(*it) == &v.first);
        }
        SECTION("Access")
        {
            SECTION("Access any")
            {
                auto pos = GENERATE_ONE(size_t(0), size - 1);
                auto ptr = &vals.at(pos);
                auto it = cache.toIter(ptr);
                REQUIRE(it != cache.cend());
                cache.access(it);
                auto it1 = --cache.cend();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    --it1;
                }
            }
            SECTION("Access first")
            {
                cache.access(cache.begin());
                auto it1 = --cache.cend();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    --it1;
                }
            }
            SECTION("Access last")
            {
                cache.access(--cache.end());
                auto it1 = --cache.cend();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    --it1;
                }
            }
        }
    }
    SECTION("LRU")
    {
        cache_manager::cache_manager<int, cache_manager::weight<int>, cache_manager::policy::lru<int>> cache(2048);
        fill(cache, vals);
        SECTION("Erase")
        {
            REQUIRE(cache.next() == cache.cbegin());
        }
        SECTION("Insert")
        {
            cache_manager::stored_node<int> v {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
            auto it = cache.insert(&v);
            REQUIRE(it == --cache.cend());
            REQUIRE(&(*it) == &v.first);
        }
        SECTION("Access")
        {
            SECTION("Access any")
            {
                auto pos = GENERATE_ONE(size_t(0), size - 1);
                auto ptr = &vals.at(pos);
                auto it = cache.toIter(ptr);
                REQUIRE(it != cache.cend());
                cache.access(it);
                REQUIRE(it == --cache.cend());
                auto it1 = cache.cbegin();
                for(auto &i : vals)
                {
                    if(&i.first != &*it)
                    {
                        REQUIRE(&i.first == &*it1);
                        ++it1;
                    }
                }
            }
            SECTION("Access first")
            {
                cache.access(cache.begin());
                auto it1 = cache.cbegin();
                REQUIRE(&vals.at(0).first == &*(--cache.cend()));
                for(auto vit = ++vals.cbegin(); vit != vals.cend(); ++vit)
                {
                    REQUIRE(&vit->first == &*it1);
                    ++it1;
                }
            }
            SECTION("Access last")
            {
                cache.access(--cache.end());
                auto it1 = cache.begin();
                for (auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    ++it1;
                }
            }
        }
    }
    SECTION("MRU")
    {
        cache_manager::cache_manager<int, cache_manager::weight<int>, cache_manager::policy::mru<int>> cache(2048);
        fill(cache, vals);
        SECTION("Erase")
        {
            REQUIRE(cache.next() == cache.cbegin());
        }
        SECTION("Insert")
        {
            cache_manager::stored_node<int> v {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
            auto it = cache.insert(&v);
            REQUIRE(it == cache.cbegin());
            REQUIRE(&(*it) == &v.first);
        }
        SECTION("Access")
        {
            SECTION("Access any")
            {
                auto pos = GENERATE_ONE(size_t(0), size - 1);
                auto ptr = &vals.at(pos);
                auto it = cache.toIter(ptr);
                REQUIRE(it != cache.cend());
                cache.access(it);
                REQUIRE(it == cache.cbegin());
                auto it1 = --cache.cend();
                for(auto &i : vals)
                {
                    if(&i.first != &*it)
                    {
                        REQUIRE(&i.first == &*it1);
                        --it1;
                    }
                }
            }
            SECTION("Access first")
            {
                cache.access(cache.begin());
                auto it1 = --cache.cend();
                for (auto &i : vals)
                {
                    REQUIRE(&i.first == &*it1);
                    --it1;
                }
            }
            SECTION("Access last")
            {
                cache.access(--cache.end());
                auto it1 = --cache.cend();
                REQUIRE(&vals.at(0).first == &*(cache.cbegin()));
                for(auto vit = ++vals.cbegin(); vit != vals.cend(); ++vit)
                {
                    REQUIRE(&vit->first == &*it1);
                    --it1;
                }
            }
        }
    }

    SECTION("Swapping")
    {
        cache_manager::cache_manager<int, cache_manager::weight<int>, cache_manager::policy::swapping<int>> cache(2048);
        fill(cache, vals);
        SECTION("Erase")
        {
            REQUIRE(cache.next() == cache.cbegin());
        }
        SECTION("Insert")
        {
            cache_manager::stored_node<int> v {GENERATE_ONE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), {}};
            auto it = cache.insert(&v);
            REQUIRE(it == cache.cbegin());
            REQUIRE(&(*it) == &v.first);
        }
        SECTION("Access")
        {
            SECTION("Access any")
            {
                auto pos = GENERATE_ONE(size_t(0), size - 2);
                auto ptr = &vals.at(pos + 1);
                auto it1 = cache.toIter(ptr);
                auto it2 = it1;
                ++it2;
                REQUIRE(it1 != cache.cend());
                REQUIRE(it2 != cache.cend());
                cache.access(it1);
                auto it = it2;
                ++it;
                REQUIRE(it == it1);
                it = --cache.end();
                for(auto &i : vals)
                {
                    if(it != it1 && it != it2)
                    {
                        REQUIRE(&i.first == &*it);
                    }
                    --it;
                }
            }
            SECTION("Access first")
            {
                auto it1 = cache.begin();
                cache.access(it1);
                REQUIRE(++cache.begin() == it1);
                auto it = --cache.cend();
                for(auto &i : vals)
                {
                    if(it != it1 && it != cache.cbegin())
                    {
                        REQUIRE(&i.first == &*it);
                    }
                    --it;
                }
            }
            SECTION("Access last")
            {
                auto it1 = --cache.end();
                cache.access(it1);
                REQUIRE(it1 == --cache.cend());
                auto it = --cache.cend();
                for(auto &i : vals)
                {
                    REQUIRE(&i.first == &*it);
                    --it;
                }
            }
        }
    }
    SECTION("Simple locked policy")
    {
        SECTION("Locked middle")
        {
            auto pos = GENERATE_ONE(size_t(1), size - 2);
            auto ptr = &vals.at(pos).first;
            cache_manager::policy::locked_policy<int, cache_manager::policy::fifo<int>,
                    std::function<bool(const int&)>>
                    policy([ptr](const int& val)
                    {
                        return &val == ptr;
                    });
            cache_manager::cache_manager<int, cache_manager::weight<int>,
                    cache_manager::policy::locked_policy<int, cache_manager::policy::fifo<int>,
                    std::function<bool(const int&)>>>
                    cache(2048, cache_manager::weight<int>(), policy);
            fill(cache, vals);
            REQUIRE(cache.next() == cache.cbegin());
        }
        SECTION("Locked begin")
        {
            auto num = GENERATE_ONE(size_t(1), size - 1);
            cache_manager::policy::locked_policy<int, cache_manager::policy::fifo<int>,
                std::function<bool(const int&)>>
                policy([&vals, num](const int& val)
                {
                    for(size_t i = 0; i < num; ++i)
                    {
                        if(&vals.at(i).first == &val)
                        {
                            return true;
                        }
                    }
                    return false;
                });
            cache_manager::cache_manager<int, cache_manager::weight<int>,
                    cache_manager::policy::locked_policy<int, cache_manager::policy::fifo<int>,
                            std::function<bool(const int&)>>>
                    cache(2048, cache_manager::weight<int>(), policy);
            fill(cache, vals);
            REQUIRE(&*cache.next() == &vals.at(num).first);
        }
        SECTION("All locked")
        {
            cache_manager::policy::locked_policy<int, cache_manager::policy::fifo<int>,
                    std::function<bool(const int&)>>
                    policy([](const int&)
                           {
                               return true;
                           });
            cache_manager::cache_manager<int, cache_manager::weight<int>,
                    cache_manager::policy::locked_policy<int, cache_manager::policy::fifo<int>,
                            std::function<bool(const int&)>>>
                    cache(2048, cache_manager::weight<int>(), policy);
            fill(cache, vals);
            REQUIRE(cache.next() == cache.cend());
        }
    }
    SECTION("Priority locked policy")
    {
        SECTION("Completely locked")
        {
            SECTION("Locked middle")
            {
                auto pos = GENERATE_ONE(size_t(1), size - 2);
                auto ptr = &vals.at(pos);
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([ptr](const int& val)
                               {
                                   return &val == &ptr->first ? std::numeric_limits<size_t>::max() : std::numeric_limits<size_t>::min();
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(cache.next() == cache.cbegin());
            }
            SECTION("Locked begin")
            {
                auto num = GENERATE_ONE(size_t(1), size - 1);
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([&vals, num](const int& val)
                               {
                                   for(size_t i = 0; i < num; ++i)
                                   {
                                       if(&vals.at(i).first == &val)
                                       {
                                           return std::numeric_limits<size_t>::max();
                                       }
                                   }
                                   return std::numeric_limits<size_t>::min();
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(&*cache.next() == &vals.at(num).first);
            }
            SECTION("All locked")
            {
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([](const int&)
                               {
                                   return std::numeric_limits<size_t>::max();
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(cache.next() == cache.cend());
            }
        }
        SECTION("Priority locked")
        {
            SECTION("Search some")
            {
                auto num = GENERATE_ONE(size_t(1), size - 1);
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([num](const int&)
                               {
                                   return num;
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(&*cache.next() == &vals.front().first);
            }
            SECTION("Search past end")
            {
                auto num = GENERATE_ONE(size + 1, 2*size);
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([num](const int)
                               {
                                   return num;
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(&*cache.next() == &vals.front().first);
            }
            SECTION("Search and find more efficient")
            {
                auto num = GENERATE_ONE(size_t(5), size - 1);
                auto pos = GENERATE_ONE(std::numeric_limits<size_t>::min(), std::numeric_limits<size_t>::max())%(num - 3) + 1;
                auto ptr = &vals.at(pos);
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([num, ptr](const int& val)
                               {
                                   return &ptr->first == &val ? 1 : num;
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(&*cache.next() == &ptr->first);
            }
            SECTION("Search and find more efficient with offset")
            {
                auto offset = GENERATE_ONE(size_t(1), size/2);
                auto num = GENERATE_ONE(std::numeric_limits<size_t>::min(), std::numeric_limits<size_t>::max())%(vals.size() - offset - 5) + 5;
                auto pos = GENERATE_ONE(std::numeric_limits<size_t>::min(), std::numeric_limits<size_t>::max())%(num - 3) + 1;
                auto ptr = &vals.at(pos + offset);
                cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                        std::function<size_t(const int&)>>
                        policy([num, ptr, &vals, offset](const int& val)
                               {
                                   for(size_t i = 0; i < offset; ++i)
                                   {
                                       if(&vals.at(i).first == &val)
                                       {
                                           return std::numeric_limits<size_t>::max();
                                       }
                                   }
                                   return &ptr->first == &val ? 1 : num;
                               });
                cache_manager::cache_manager<int, cache_manager::weight<int>,
                        cache_manager::policy::priority_policy<int, cache_manager::policy::fifo<int>,
                                std::function<size_t(const int&)>>>
                        cache(2048, cache_manager::weight<int>(), policy);
                fill(cache, vals);
                REQUIRE(&*cache.next() == &ptr->first);
            }
        }
    }
}
