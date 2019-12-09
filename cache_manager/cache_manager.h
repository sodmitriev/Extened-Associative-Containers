// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef EXTENDEDASSOCIATIVECONTAINERS_CACHE_MANAGER_H
#define EXTENDEDASSOCIATIVECONTAINERS_CACHE_MANAGER_H

#include "base.h"
#include "replacement_iterator.h"
#include "policy.h"

#include <algorithm>

namespace cache_manager
{

    /*!
     * Class that manages cache policies of stored values
     * @tparam T Managed value type
     * @tparam Weight Functor that calculates weight of stored objects
     * @tparam RepPolicy Desired replacement policy
     */
    template<
            class T,
            class Weight = weight<T>,
            class RepPolicy = policy::lru<T>
    >
    class cache_manager
    {
    private:

        size_t _capacity;
        stored_node<T> _first = {T{}, {&_first, &_first}};
        size_t _weight;

        Weight _weigher;
        RepPolicy _policy;

        void _fix_first(const stored_node<T>* otherFirst)
        {
            if(__next(&_first) == otherFirst)
            {
                assert(__next(&_first) == __prev(&_first));
                __link(&_first, &_first);
            }
            else
            {
                __link(&_first, __next(&_first));
                __link(__prev(&_first), &_first);
            }
        }

    public:

        typedef size_t weight_type;
        typedef Weight weigher_type;
        typedef RepPolicy policy_type;
        typedef replacement_iterator<T> iterator;
        typedef const_replacement_iterator<T> const_iterator;

        /*!
         * Default constructor
         */
        cache_manager() : _capacity(0), _weight(0) {};

        /*!
         * Constructor
         * @param capacity Cache capacity
         * @param weigher Functor that calculates weight of stored objects
         * @param policy Desired replacement policy
         */
        explicit cache_manager(size_t capacity, const Weight& weigher = Weight(), const RepPolicy& policy = RepPolicy())
                : _capacity(capacity), _weight(0), _weigher(weigher), _policy(policy)
                {}

        cache_manager(const cache_manager&) = delete;

        /*!
         * Move constructor
         */
        cache_manager(cache_manager&& other) noexcept(std::is_nothrow_move_constructible_v<Weight>
                                                      && std::is_nothrow_move_constructible_v<RepPolicy>)
                : _capacity(other._capacity),
                  _first(other._first),
                  _weight(other._weight),
                  _weigher(std::move(other._weigher)),
                  _policy(std::move(other._policy))
        {
            _fix_first(&other._first);
        }

        cache_manager& operator=(const cache_manager&) = delete;

        cache_manager& operator=(cache_manager&& other) noexcept(std::is_nothrow_move_constructible_v<Weight>
                                                                 && std::is_nothrow_move_constructible_v<RepPolicy>)
        {
            _capacity = other._capacity;
            other._capacity = 0;
            _first.second = std::move(other._first.second);
            __link(&other._first, &other._first);
            _weight = other._weight;
            other._weight = 0;
            _weigher = std::move(other._weigher);
            _policy = std::move(other._policy);
            _fix_first(&other._first);
            return *this;
        }

        /*!
         * Get weight functor used by cache manager
         * @return Weight functor used by cache manager
         */
        Weight get_weigher() const { return _weigher; }

        /*!
         * Get replacement policy used by cache manager
         * @return Replacement policy used by cache manager
         */
        RepPolicy get_policy() const { return _policy; }

        /*!
         * Get iterator pointing to replacement list beginning
         * @return replacement_iterator pointing to replacement list beginning
         */
        replacement_iterator<T> begin() { return ++replacement_iterator<T>(&_first); }

        /*!
         * Get const iterator pointing to replacement list beginning
         * @return Const iterator pointing to replacement list beginning
         */
        const_replacement_iterator<T> begin() const { return ++const_replacement_iterator<T>(&_first); }

        /*!
         * Get const iterator pointing to replacement list beginning
         * @return Const iterator pointing to replacement list beginning
         */
        const_replacement_iterator<T> cbegin() const { return ++const_replacement_iterator<T>(&_first); }

        /*!
         * Get iterator pointing to replacement list ending
         * @return replacement_iterator pointing to replacement list ending
         */
        replacement_iterator<T> end() { return replacement_iterator<T>(&_first); }

        /*!
         * Get const iterator pointing to replacement list ending
         * @return Const iterator pointing to replacement list ending
         */
        const_replacement_iterator<T> end() const { return const_replacement_iterator<T>(&_first); }

        /*!
         * Get const iterator pointing to replacement list ending
         * @return Const iterator pointing to replacement list ending
         */
        const_replacement_iterator<T> cend() const { return const_replacement_iterator<T>(&_first); }

        /*!
         * Returns the total weight of currently managed elements
         * @return Total weight of currently managed elements
         */
        size_t weight() const { return _weight; }

        /*!
         * Returns the maximum total weight of managed elements
         * @return Maximum total weight of managed elements
         */
        size_t capacity() const { return _capacity; }

        /*!
         * Set the maximum total weight of managed elements
         * @param capacity Maximum total weight of managed elements
         */
        void set_capacity(size_t capacity)
        {
            assert(capacity >= _weight);
            _capacity = capacity;
        }

        /*!
         * Returns weight of a value in case a value is already managed by cache manager
         * This overload might work faster with complex weigher function
         * @param val Value to check weight of
         * @return Weight of value
         */
        size_t calculate_weight(std::add_lvalue_reference_t<std::add_const_t<T>> val) const
        {
            return _weigher(val);
        }

        /*!
         * Returns weight of a value indexed by cache manager
         * @param val Value to check weight of
         * @return Weight of value
         */
        size_t calculate_weight(const_replacement_iterator<T> it) const
        {
#ifdef CACHE_STORE_WEIGHT
            return it._ptr->second.weight;
#else
            return _weigher(it._ptr->first);
#endif
        }

        /*!
         * Checks whether manager can fit a value with provided weight
         * @param weight Weight of a value
         * @return True if manager can fit a value of provided weight, false otherwise
         */
        bool can_fit(size_t weight) const { return (_weight + weight) <= _capacity; }

        /*!
         * Checks whether manager can fit the provided value
         * @param val Value to check
         * @return True if manager can fit the provided value, false otherwise
         */
        bool can_fit(std::add_lvalue_reference_t<std::add_const_t<T>> val) const { return can_fit(calculate_weight(val)); }

        /*!
         * Clears the contents
         */
        void clear() noexcept
        {
            __link(&_first, &_first);
            _weight = 0;
        }

        /*!
         * Inserts a new value that will be managed by this manager
         * @param value Value to insert
         * @param node To use in list
         * @return replacement_iterator to newly inserted value
         */
        replacement_iterator<T> insert(stored_node<T>* node)
        {
            assert(node != nullptr);
            auto weight = calculate_weight(node->first);
            assert(can_fit(weight));
            auto place = policy::policy_extractor<T, RepPolicy>::insert_position(_policy, begin(), end());
            auto * prev = __prev(place._ptr);
            //place._ptr is a const path pointing to non const object
            auto * next = const_cast<stored_node<T>*>(place._ptr);
            __link(prev, node);
            __link(node, next);
            _weight += weight;
#if !defined(NDEBUG) || defined(CACHE_STORE_WEIGHT)
            node->second.weight = weight;
#endif
            return replacement_iterator<T>(node);
        }

        /*!
         * Inserts an element that was previously erased from manager
         * Must be performed in an order opposite to erase and no manager modifications must have happened between
         * erase and reinsert
         * @param it Iterator to an element that was previously erased from manager
         */
        void reinsert(const_replacement_iterator<T> it)
        {
            auto weight = calculate_weight(it);
            assert(can_fit(weight));
            _weight += weight;
            auto prev = __prev(it._ptr);
            auto next = __next(it._ptr);
            __link(prev, const_cast<stored_node<T>*>(it._ptr));
            __link(const_cast<stored_node<T>*>(it._ptr), next);
        }


        /*!
         * Updates weight of a changed element
         * @param it Changed element
         * @param oldWeight Weight before change
         * @param newWeight Weight after change
         */
        void update_weight(replacement_iterator<T> it, size_t oldWeight, size_t newWeight)
        {
            assert(newWeight == calculate_weight(*it));
            _weight -= oldWeight;
            _weight += newWeight;
            assert(_weight <= _capacity);
#if !defined(NDEBUG) || defined(CACHE_STORE_WEIGHT)
            assert(it._ptr->second.weight == oldWeight);
            it._ptr->second.weight = newWeight;
#endif
        }

        /*!
         * Updates weight of a changed element
         * @param it Changed element
         * @param oldWeight Weight before change
         */
        void update_weight(replacement_iterator<T> it, size_t oldWeight)
        {
            update_weight(it, oldWeight, calculate_weight(*it));
        }

        /*!
         * Erase element from manager
         * @param node Pointer to node to erase
         * @return Amount of erased elements
         */
        void erase(const stored_node<T>* node)
        {
            auto prev = __prev(node);
            auto next = __next(node);
            __link(prev, next);
            auto weight =
#ifdef CACHE_STORE_WEIGHT
                node->weight;
#else
                    calculate_weight(node->first);
#endif
#ifndef NDEBUG
            assert(weight == node->second.weight);
#endif
            _weight -= weight;
        }

        /*!
         * Erase element from manager
         * @param replacementIt replacement_iterator pointing to the element to erase
         * @return replacement_iterator following the last removed element
         */
        replacement_iterator<T> erase(const_replacement_iterator<T> replacementIt)
        {
            auto next = __next(replacementIt._ptr);
            erase(replacementIt._ptr);
            return replacement_iterator<T>(next);
        }

        /*!
         * Erase range of elements from manager
         * @param first Beginning of the range
         * @param first Ending of the range
         * @return replacement_iterator following the last removed element
         */
        replacement_iterator<T> erase(const_replacement_iterator<T> first, const_replacement_iterator<T> last)
        {
            auto prev = __prev(first._ptr);
            auto next = const_cast<stored_node<T>*>(last._ptr);
            size_t sumWeight = 0;
            while(first != last)
            {
                auto node = first._ptr;
                auto weight =
#ifdef CACHE_STORE_WEIGHT
                        node->weight;
#else
                        calculate_weight(node->first);
#endif
#ifndef NDEBUG
                assert(weight == node->second.weight);
#endif
                sumWeight += weight;
                ++first;
            }
            _weight -= sumWeight;
            __link(prev, next);
            return replacement_iterator<T>(next);
        }

        /*!
         * Convert node pointer to iterator
         * @param node Pointer to a node of the cache
         * @return replacement_iterator to provided node
         */
        static replacement_iterator<T> toIter(stored_node<T>* node)
        {
            return replacement_iterator<T>(node);
        }

        /*!
         * Convert node pointer to iterator
         * @param node Pointer to a node of the cache
         * @return Constant iterator to provided node
         */
        static const_replacement_iterator<T> toIter(const stored_node<T> *node)
        {
            return const_replacement_iterator<T>(node);
        }

        /*!
         * Modify replacement order list based on replacement policy
         * @param replacementIt Pointer to element that was accessed
         */
        void access(stored_node<T>* node)
        {
            access(replacement_iterator<T>(node));
        }

        /*!
         * Modify replacement order list based on replacement policy
         * @param replacementIt replacement_iterator pointing to element that was accessed
         */
        void access(replacement_iterator<T> replacementIt)
        {
            policy::policy_extractor<T, RepPolicy>::access(_policy, begin(), end(), replacementIt);
        }

        /*!
         * Get next element to be erased according to replacement policy
         * @return replacement_iterator pointing to an element that should be erased next, end iterator it none can be erased
         */
        const_replacement_iterator<T> next() const
        {
            if(cbegin() == cend())
            {
                return cend();
            }
            return policy::policy_extractor<T, RepPolicy>::erase_position(_policy, cbegin(), cend());
        }

        /*!
         * Get next element to be erased according to replacement policy, starting from hint position
         * @param hint Position to start search for element to erase
         * @return replacement_iterator pointing to an element that should be erased next, end iterator it none can be erased
         */
        const_replacement_iterator<T> next(const_replacement_iterator<T> hint) const
        {
            if(hint == cend())
            {
                return cend();
            }
            return policy::policy_extractor<T, RepPolicy>::erase_position(_policy, hint, cend());
        }

        /*!
         * Get next element to be erased according to replacement policy, ignoring the provided element
         * @param except Element to ignore when searching for next
         * @return replacement_iterator pointing to an element that should be erased next, end iterator it none can be erased
         */
        const_replacement_iterator<T> next_except(const_replacement_iterator<T> except) const
        {
            if(cbegin() == cend())
            {
                return cend();
            }
            auto prev = __prev(except._ptr);
            auto next = __next(except._ptr);
            __link(prev, next);
            auto ret = policy::policy_extractor<T, RepPolicy>::erase_position(_policy, cbegin(), cend());
            assert(ret != except);
            __link(prev, const_cast<stored_node<T>*>(except._ptr));
            __link(const_cast<stored_node<T>*>(except._ptr), next);
            return ret;
        }

        /*!
         * Get next element to be erased according to replacement policy, starting from hint position and ignoring the provided element
         * @param hint Position to start search for element to erase
         * @param except Element to ignore when searching for next
         * @return replacement_iterator pointing to an element that should be erased next, end iterator it none can be erased
         */
        const_replacement_iterator<T> next_except(const_replacement_iterator<T> hint, const_replacement_iterator<T> except) const
        {
            if(cbegin() == cend())
            {
                return cend();
            }
            if(hint == except)
            {
                ++hint;
            }
            auto prev = __prev(except._ptr);
            auto next = __next(except._ptr);
            __link(prev, next);
            auto ret = policy::policy_extractor<T, RepPolicy>::erase_position(_policy, hint, cend());
            assert(ret != except);
            __link(prev, const_cast<stored_node<T>*>(except._ptr));
            __link(const_cast<stored_node<T>*>(except._ptr), next);
            return ret;
        }

        /*!
         * Swaps content of two managers
         * @param other Other manager
         */
        void swap(cache_manager& other) noexcept(std::is_nothrow_swappable_v<Weight> && std::is_nothrow_swappable_v<RepPolicy> )
        {
            std::swap(_first.second, other._first.second);
            _fix_first(&other._first);
            other._fix_first(&_first);
            std::swap(_capacity, other._capacity);
            std::swap(_weight, other._weight);
            std::swap(_weigher, other._weigher);
            std::swap(_policy, other._policy);
        }
        
    };

}


#endif //EXTENDEDASSOCIATIVECONTAINERS_CACHE_MANAGER_H
