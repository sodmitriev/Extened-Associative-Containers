// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef EXTENDEDASSOCIATIVECONTAINERS_POLICY_H
#define EXTENDEDASSOCIATIVECONTAINERS_POLICY_H

#include "replacement_iterator.h"

#include <cstddef>
#include <type_traits>

namespace cache_manager
{

    namespace policy
    {

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // defined(__GNUC__)


        /*!
         * Template helper to check if a policy specifies custom insert position
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template <class T, class Policy, class = void>
        struct provides_insert
        {
        private:
            template <class T1, class Policy1>
            static constexpr auto test(int) -> decltype(std::enable_if_t<std::is_same_v<const_replacement_iterator<T1>,
                    decltype(std::declval<Policy1>().insert_position(std::declval<const_replacement_iterator<T1>>(),
                            std::declval<const_replacement_iterator<T1>>()))>>(), std::true_type());
            template <class, class>
            static constexpr std::false_type test(...);
        public:
            static constexpr bool value = decltype(test<T, Policy>(0))::value;
        };

        template <class T, class Policy>
        struct provides_insert<T, Policy, std::enable_if_t<!std::is_class_v<Policy>>>
        {
            static constexpr bool value = false;
        };

        /*!
         * Template helper to check if a policy specifies custom insert position
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template<class T, class Policy>
        constexpr bool provides_insert_v = provides_insert<T, Policy>::value;

        /*!
         * Template helper to check if a policy specifies custom erase position
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template <class T, class Policy, class = void>
        struct provides_erase
        {
        private:
            template <class T1, class Policy1>
            static constexpr auto test(int) -> decltype(std::enable_if_t<std::is_same_v<const_replacement_iterator<T1>,
                    decltype(std::declval<Policy1>().erase_position(std::declval<const_replacement_iterator<T1>>(),
                    std::declval<const_replacement_iterator<T1>>()))>>(), std::true_type());
            template <class, class>
            static constexpr std::false_type test(...);
        public:
            static constexpr bool value = decltype(test<T, Policy>(0))::value;
        };

        template <class T, class Policy>
        struct provides_erase<T, Policy, std::enable_if_t<!std::is_class_v<Policy>>>
        {
            static constexpr bool value = false;
        };

        /*!
         * Template helper to check if a policy specifies custom erase position
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template<class T, class Policy>
        constexpr bool provides_erase_v = provides_erase<T, Policy>::value;

        /*!
         * Template helper to check if a policy specifies required manipulations when an element is accessed
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template <class T, class Policy, class = void>
        struct provides_access
        {
        private:
            template <class T1, class Policy1>
            static constexpr auto test(int) -> decltype(
                    std::declval<Policy1>().access(std::declval<replacement_iterator<T1>>(),
                    std::declval<replacement_iterator<T1>>(), std::declval<replacement_iterator<T1>>()), std::true_type());
            template <class, class>
            static constexpr std::false_type test(...);
        public:
            static constexpr bool value = decltype(test<T, Policy>(0))::value;
        };

        template <class T, class Policy>
        struct provides_access<T, Policy, std::enable_if_t<!std::is_class_v<Policy>>>
        {
            static constexpr bool value = false;
        };

        /*!
         * Template helper to check if a policy specifies required manipulations when an element is accessed
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template<class T, class Policy>
        constexpr bool provides_access_v = provides_access<T, Policy>::value;

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#endif // defined(__GNUC__) && !defined(__clang__)

        /*!
         * Class to extract required operations from a custom policy
         * @tparam T Stored value type
         * @tparam Policy Policy type
         */
        template<class T, class Policy>
        struct policy_extractor
        {

            /*!
             * Calculates the element before which the new element will be inserted
             * @note If provided policy does not specify custom insert position, returns last
             * @param policy Used replacement policy
             * @param first The beginning of the element range
             * @param last The end of the element range
             * @return The element before which the new element will be inserted
             */
            static const_replacement_iterator<T> insert_position(const Policy& policy, const_replacement_iterator<T> first, const_replacement_iterator<T> last)
            {
                if constexpr (provides_insert_v<T, Policy>)
                {
                    return policy.insert_position(first, last);
                }
                else
                {
                    return last;
                }
            }



            /*!
             * Calculates the element that should be erased next
             * @note If provided policy does not specify custom erase position, returns first
             * @param policy Used replacement policy
             * @param first The beginning of the element range
             * @param last The end of the element range
             * @return The element that should be erased next
             */
            static const_replacement_iterator<T> erase_position(const Policy& policy, const_replacement_iterator<T> first, const_replacement_iterator<T> last)
            {
                if constexpr (provides_erase_v<T, Policy>)
                {
                    return policy.erase_position(first, last);
                }
                else
                {
                    return first;
                }
            }

            /*!
             * Modifies accessed element position according to replacement algorithm
             * @note If provided policy does not specify custom access operation, container is not modified
             * @param policy Used replacement policy
             * @param first The beginning of the element range
             * @param last The end of the element range
             * @param accessed The element that was accessed
             */
            static void access(Policy& policy, replacement_iterator<T> first, replacement_iterator<T> last, replacement_iterator<T> accessed)
            {
                if constexpr (provides_access_v<T, Policy>)
                {
                    policy.access(first, last, accessed);
                }
            }
        };

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif // defined(__GNUC__) && !defined(__clang__)

        template<class T>
        struct example_replacement_algorithm
        {

            /*!
             * Calculates the element before which the new element will be inserted
             * @param first The beginning of the element range
             * @param last The end of the element range
             * @return The element before which the new element will be inserted
             */
            const_replacement_iterator<T> insert_position(const_replacement_iterator<T> first, const_replacement_iterator<T> last) const
            {
                return first;
            }

            /*!
             * Calculates the element that should be erased next
             * @param first The beginning of the element range
             * @param last The end of the element range
             * @return The element that should be erased next
             */
            const_replacement_iterator<T> erase_position(const_replacement_iterator<T> first, const_replacement_iterator<T> last) const
            {
                return first;
            }

            /*!
             * Modifies accessed element position according to replacement algorithm
             * @param first The beginning of the element range
             * @param last The end of the element range
             * @param accessed The element that was accessed
             */
            void access(replacement_iterator<T> first, replacement_iterator<T> last, replacement_iterator<T> accessed) {}
        };

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif // defined(__GNUC__)

        template<class T>
        struct fifo {}; //Default policy behaviour

        template<class T>
        struct lifo
        {
            const_replacement_iterator<T> insert_position(const_replacement_iterator<T> first, const_replacement_iterator<T>) const
            {
                return first;
            }
        };

        template<class T>
        struct lru
        {
            void access(replacement_iterator<T>, replacement_iterator<T> last, replacement_iterator<T> accessed)
            {
                auto next = accessed;
                ++next;
                if(next != last)
                {
                    iterator_operations::move<T>(accessed, next, last);
                }
            }
        };

        template<class T>
        struct mru
        {
            const_replacement_iterator<T> insert_position(const_replacement_iterator<T> first, const_replacement_iterator<T>) const
            {
                return first;
            }

            void access(replacement_iterator<T> first, replacement_iterator<T>, replacement_iterator<T> accessed)
            {
                if(accessed != first)
                {
                    auto next = accessed;
                    ++next;
                    iterator_operations::move<T>(accessed, next, first);
                }
            }
        };

        template<class T>
        struct swapping
        {
            const_replacement_iterator<T> insert_position(const_replacement_iterator<T> first, const_replacement_iterator<T>) const
            {
                return first;
            }

            void access(replacement_iterator<T>, replacement_iterator<T> last, replacement_iterator<T> accessed)
            {
                auto next = accessed;
                ++next;
                if(next != last)
                {
                    iterator_operations::iter_swap<T>(next, accessed);
                }
            }
        };

        /*!
         * Policy adaptor that implements element locking
         *
         * Will never erase elements that are locked. Policy moves from first to last element in search of the first
         * unlocked element. This behaviour may result if O(N) difficulty of erase if all elements are locked.
         *
         * @tparam T Stored value type
         * @tparam ParentPolicy Basic policy, should not specify erase position
         * @tparam Locker Functor to check if a element is locked (refer to SimpleLocker for examples)
         */
        template<class T, class ParentPolicy, class Locker>
        struct locked_policy : public ParentPolicy
        {
        private:
            Locker _locker;
        public:

            static_assert(!provides_erase_v<T, ParentPolicy>,
                    "Can't apply locked policy when parent policy specifies custom erase position");

            template <class ... Args>
            explicit locked_policy(const Locker& locker = Locker(), Args && ... args)
                : ParentPolicy(std::forward<Args>(args)...), _locker(locker) {}

            const_replacement_iterator<T> erase_position(const_replacement_iterator<T> first, const_replacement_iterator<T> last) const
            {
                for(auto it = first; it != last; ++it)
                {
                    if(!_locker(*it))
                    {
                        return it;
                    }
                }
                return last;
            }

        };

        /*!
         * Example locker class that corresponds to simple locked policy
         * @tparam T Stored element type
         */
        template<class T>
        struct locker
        {
            /*!
             * Check if element is locked
             * @param iterator replacement_iterator pointing to checked element
             * @return true if element is locked, false otherwise
             */
            bool operator()(const T&) { return false; }
        };

        /*!
         * Policy adaptor that implements element locking based on priority
         *
         * Policy will never erase elements with priority equal to numeric_limits::max() of weight type returned by
         * Locker::operator(). Elements with priority of numeric_limits::min() will be erased as soon as encountered
         * while moving from first to last. If element with position N if replacement list has a priority of P, policy
         * will search for an element with lesser priority in range (N, N + P]. If another element has a position N + M
         * and a priority of A where M <= P and A <= P - M then element at position N + M becomes current and algorithm
         * repeats for this element. If there is no such element in range (N, N+P] then current element is erased.
         *
         * @tparam T Stored value type
         * @tparam ParentPolicy Basic policy, should not specify erase position
         * @tparam Locker Functor to get element's priority (refer to PriorityLocker for examples)
         */
        template<class T, class ParentPolicy, class Locker>
        struct priority_policy : public ParentPolicy
        {
        private:
            Locker _locker;
        public:

            static_assert(!provides_erase_v<T, ParentPolicy>,
                          "Can't apply locked policy when parent policy specifies custom erase position");

            template <class ... Args>
            explicit priority_policy(const Locker& locker = Locker(), Args && ... args)
                    : ParentPolicy(std::forward<Args>(args)...), _locker(locker) {}

            const_replacement_iterator<T> erase_position(const_replacement_iterator<T> first, const_replacement_iterator<T> last) const
            {
                assert(first != last);
                const_replacement_iterator<T> lowestPriorityIter = first;
                while(_locker(*lowestPriorityIter) == std::numeric_limits<decltype(_locker(*lowestPriorityIter))>::max())
                {
                    ++lowestPriorityIter;
                    if(lowestPriorityIter == last)
                    {
                        return last;
                    }
                }

                auto it = lowestPriorityIter;
                auto currentPriority = _locker(*lowestPriorityIter);
                while(currentPriority > std::numeric_limits<decltype(currentPriority)>::min() && it != last)
                {
                    auto priority = _locker(*it);
                    if(priority < currentPriority)
                    {
                        lowestPriorityIter = it;
                        currentPriority = priority;
                    }
                    else
                    {
                        --currentPriority;
                    }
                    ++it;
                }
                return lowestPriorityIter;
            }

        };

        /*!
         * Example locker class that corresponds to priority locked policy
         * @tparam T Stored element type
         */
        template<class T>
        struct priority
        {
            /*!
             * Returns element priority
             * @param iterator replacement_iterator pointing to checked element
             * @return Priority of element pointed by iterator
             */
            size_t operator()(const T&) { return 0; }
        };
    }
}

#endif //EXTENDEDASSOCIATIVECONTAINERS_POLICY_H
