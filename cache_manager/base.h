#ifndef EXTENDEDASSOCIATIVECONTAINERS_CACHEMANAGER_BASE_H
#define EXTENDEDASSOCIATIVECONTAINERS_CACHEMANAGER_BASE_H

#include <cstddef>
#include <utility>

namespace cache_manager
{

    /*!
     * Simple size functor that returns one for every value
     * @tparam T Managed value type
     */
    template <class T>
    struct weight
    {
    public:
        size_t operator()(std::add_lvalue_reference_t<std::add_const_t<T>>) const { return 1; }
    };

    /*!
     * Type of a node in replacement queue
     * @tparam T Type of managed values
     */
    template <class T>
    class cache_node;

    /*!
     * Type of a value stored in a limited container
     * @tparam T Type of managed values
     */
    template <class T>
    using stored_node = std::pair<const T, cache_node<T>>;

    template <class T>
    class cache_node
    {
        stored_node<T>* prev = nullptr;
        stored_node<T>* next = nullptr;
    public:

#if !defined(NDEBUG) || defined(CACHE_STORE_WEIGHT)
        size_t weight = 0;          ///< Weight of stored object
#endif

        cache_node() = default;
        cache_node(stored_node<T>* prev, stored_node<T>* next) : prev(prev), next(next) {}

        template<class T1> friend stored_node<T1>*& __prev(stored_node<T1>* node);
        template<class T1> friend stored_node<T1>*& __next(stored_node<T1>* node);
        template<class T1> friend stored_node<T1>* __prev(const stored_node<T1>* node);
        template<class T1> friend stored_node<T1>* __next(const stored_node<T1>* node);
        template<class T1> friend void __link(stored_node<T1>* first, stored_node<T1>* second);
    };

    template<class T>
    stored_node<T> *&__prev(stored_node<T> *node)
    {
        return node->second.prev;
    }

    template<class T>
    stored_node<T> *&__next(stored_node<T> *node)
    {
        return node->second.next;
    }

    template<class T>
    stored_node<T> *__prev(const stored_node<T> *node)
    {
        return node->second.prev;
    }

    template<class T>
    stored_node<T> *__next(const stored_node<T> *node)
    {
        return node->second.next;
    }

    template<class T>
    void __link(stored_node<T> *first, stored_node<T> *second)
    {
        __next(first) = second;
        __prev(second) = first;
    }

    int not_covered()
    {
        return 1;
    }

}

#endif //EXTENDEDASSOCIATIVECONTAINERS_CACHEMANAGER_BASE_H
