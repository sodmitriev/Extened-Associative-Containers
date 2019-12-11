#include "../unordered_map.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>

void test(const std::unordered_map<int, int>& values, size_t capacity)
{
    std::cout << "TESTING FOR capacity = " << capacity << std::endl;
    auto start = values.begin();
    auto end = std::next(start, capacity);
    std::unordered_map<int, int> cmpMap;
    extended_containers::limited::unordered_map<int, int> map(capacity);

    std::cout << "    insert " << capacity << " elements to non full..." << std::endl;
    cmpMap.reserve(capacity);
    map.reserve(capacity);
    auto time = std::chrono::steady_clock::now();
    cmpMap.insert(start, end);
    auto diff = std::chrono::steady_clock::now() - time;
    std::cout << "        std::unordered_map took                " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
    time = std::chrono::steady_clock::now();
    map.insert(start, end);
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        extended_containers::limited::map took "
              << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
    start = end;
    end = std::next(start, capacity);

    std::cout << "    insert " << capacity << " elements to full" << std::endl;
    cmpMap.reserve(capacity * 2);
    map.reserve(capacity * 2);
    time = std::chrono::steady_clock::now();
    cmpMap.insert(start, end);
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        std::unordered_map took                " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
    time = std::chrono::steady_clock::now();
    map.insert(start, end);
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        extended_containers::limited::map took "
              << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;

    std::cout << "    finding " << capacity << " elements" << std::endl;
    cmpMap.clear();
    cmpMap.reserve(capacity);
    cmpMap.insert(map.begin(), map.end());
    time = std::chrono::steady_clock::now();
    for(auto & i : cmpMap)
    {
        cmpMap.find(i.first);
    }
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        std::unordered_map took                " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
    time = std::chrono::steady_clock::now();
    for(auto & i : cmpMap)
    {
        map.find(i);
    }
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        extended_containers::limited::map took "
              << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;

    std::cout << "    quietly finding " << capacity << " elements" << std::endl;
    time = std::chrono::steady_clock::now();
    for(auto & i : cmpMap)
    {
        cmpMap.find(i.first);
    }
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        std::unordered_map took                " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
    time = std::chrono::steady_clock::now();
    for(auto & i : cmpMap)
    {
        map.quiet_find(i);
    }
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        extended_containers::limited::map took "
              << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;

    std::cout << "    erasing " << capacity << " elements" << std::endl;
    auto vals = cmpMap;
    time = std::chrono::steady_clock::now();
    for(auto & i : vals)
    {
        cmpMap.erase(i.first);
    }
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        std::unordered_map took                " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
    time = std::chrono::steady_clock::now();
    for(auto & i : vals)
    {
        map.erase(i);
    }
    diff = std::chrono::steady_clock::now() - time;
    std::cout << "        extended_containers::limited::map took "
              << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
              << " us" << std::endl;
}

TEMPLATE_TEST_CASE_SIG("Limited unordered map performance test", "[extended containers][limited containers][unordered map][performance test]",
                       ((size_t count), count), 1024 * 1024)
{
    std::cout << "PREPARING TESTING DATA" << std::endl;
    std::unordered_map<int, int> values;
    values.reserve(count);
    while (values.size() < count)
    {
        values.emplace(GENERATE(take(1, random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()))),
                       GENERATE(take(1, random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()))));
    }
    test(values, count / 32);
    test(values, count / 16);
    test(values, count / 8);
    test(values, count / 4);
    test(values, count / 2);
}
