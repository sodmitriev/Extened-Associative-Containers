#ifndef EXTENDEDASSOCIATIVECONTAINERS_REPLACEMENTITERATOR_H
#define EXTENDEDASSOCIATIVECONTAINERS_REPLACEMENTITERATOR_H

#include "base.h"

#include <iterator>
#include <cassert>

namespace cache_manager
{

    template <class T, bool IsConst>
    using __add_const_t = std::conditional_t<IsConst, std::add_const_t<T>, T>;

    /*!
     * Replacement iterator class for cache manager
     * @tparam T Cached value
     * @tparam IsConst If true then pointers to nodes will be constant
     */
    template<class T, bool IsConst = false>
    class replacement_iterator
    {

        typedef std::add_pointer_t<__add_const_t<stored_node<T>, IsConst>> _ptr_type;
        _ptr_type _ptr = nullptr;

        explicit replacement_iterator(_ptr_type ptr)
            : _ptr(ptr) {}

        template<
                class T1,
                class Weight,
                class RepPolicy
        >
        friend class cache_manager;

        template <class T1, bool isConst1>
        friend class replacement_iterator;

        friend struct iterator_operations;

    public:

        typedef std::ptrdiff_t difference_type;
        typedef const T value_type;
        typedef const T* pointer;
        typedef const T& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        /*!
         * Constructor
         */
        replacement_iterator() = default;

        /*!
         * Copy constructor
         */
        replacement_iterator(const replacement_iterator &other) = default;

        /*!
         * Copy assignment
         */
        replacement_iterator &operator=(const replacement_iterator &other) = default;

        /*!
         * Convert non const iterator to const iterator
         */
        operator replacement_iterator<T, true>() { return replacement_iterator<T, true>(_ptr); };

        /*!
         * Swap this iterator state with other
         * @param other Other iterator
         */
        void swap(replacement_iterator &other)
        { std::swap(_ptr, other._ptr); }

        /*!
         * Dereference value stored by iterator
         * @return Value stored by iterator
         */
        reference operator*()
        {
            assert(_ptr != nullptr);
            return _ptr->first;
        }

        /*!
         * Dereference value stored by iterator
         * @return Pointed to value stored by iterator
         */
        pointer operator->()
        {
            assert(_ptr != nullptr);
            return &_ptr->first;
        }

        /*!
         * Prefix increment operator
         * @return This iterator after increment
         */
        replacement_iterator &operator++()
        {
            assert(__prev(__next(_ptr)) == _ptr);
            _ptr = __next(_ptr);
            return *this;
        }

        /*!
         * Postfix increment operator
         * @return replacement_iterator pointing to same value as this iterator before increment
         */
        replacement_iterator operator++(int)
        {
            assert(__prev(__next(_ptr)) == _ptr);
            auto tmp = *this;
            _ptr = __next(_ptr);
            return tmp;
        }

        /*!
         * Postfix decrement operator
         * @return This iterator after decrement
         */
        replacement_iterator &operator--()
        {
            assert(__next(__prev(_ptr)) == _ptr);
            _ptr = __prev(_ptr);
            return *this;
        }

        /*!
         * Postfix decrement operator
         * @return replacement_iterator pointing to same value as this iterator before decrement
         */
        replacement_iterator operator--(int)
        {
            assert(__next(__prev(_ptr)) == _ptr);
            auto tmp = *this;
            _ptr = __prev(_ptr);
            return tmp;
        }

        /*!
         * Compare this iterator with other iterator
         * @param other Other iterator
         * @return True if both iterators point to same object, false otherwise
         */
        template <bool isConst1>
        bool operator==(const replacement_iterator<T, isConst1> &other) const
        { return _ptr == other._ptr; }

        /*!
         * Compare this iterator with other iterator
         * @param other Other iterator
         * @return False if both iterators point to same object, true otherwise
         */
        template <bool isConst1>
        bool operator!=(const replacement_iterator<T, isConst1> &other) const
        { return _ptr != other._ptr; }
    };

    /*!
     * Swap two iterator states
     * @param it1 First iterator
     * @param it2 Second iterator
     */
    template<class T, bool isConst = false>
    void swap(replacement_iterator<T, isConst> &it1, replacement_iterator<T, isConst> &it2)
    { it1.swap(it2); }

    template <class T>
    using const_replacement_iterator = replacement_iterator<T, true>;

    /*!
     * Class that provides extended replacement list manipulations methods via iterators
     */
    struct iterator_operations
    {

        template<
                class T1,
                class Weight,
                class RepPolicy
        >
        friend class cache_manager;

    public:

        /*!
         * Moves a given range of elements to a specified place
         * @param first Beginning of the moved range
         * @param last Ending of the moved range, must be located after first
         * @param dest Range will be moved to a position before this element, must be located outside of [first, last] range
         */
        template <class T>
        static void move(replacement_iterator<T> first, replacement_iterator<T> last, replacement_iterator<T> dest)
        {
            auto before_ptr = __prev(first._ptr);
            auto first_ptr = first._ptr;
            auto last_ptr = __prev(last._ptr);
            auto after_ptr = last._ptr;
            auto d_before_ptr = __prev(dest._ptr);
            auto d_after_ptr = dest._ptr;

            assert(before_ptr != nullptr);
            assert(first_ptr != nullptr);
            assert(last_ptr != nullptr);
            assert(after_ptr != nullptr);
            assert(d_before_ptr != nullptr);
            assert(d_after_ptr != nullptr);

            __link(before_ptr, after_ptr);
            __link(d_before_ptr, first_ptr);
            __link(last_ptr, d_after_ptr);
        }

        /*!
         * Swaps two elements
         * @param a first element to be swapped
         * @param b second element to be swapped
         */
        template <class T>
        static void iter_swap(replacement_iterator<T> a, replacement_iterator<T> b)
        {
            auto before_a_ptr = __prev(a._ptr);
            auto a_ptr = a._ptr;
            auto after_a_ptr = __next(a._ptr);
            auto before_b_ptr = __prev(b._ptr);
            auto b_ptr = b._ptr;
            auto after_b_ptr = __next(b._ptr);

            assert(before_a_ptr != nullptr);
            assert(a_ptr != nullptr);
            assert(after_a_ptr != nullptr);
            assert(before_b_ptr != nullptr);
            assert(b_ptr != nullptr);
            assert(after_b_ptr != nullptr);

            //Only possible in looped lists if a or b is last
            assert(a_ptr != before_b_ptr || a_ptr != after_b_ptr);

            if (a_ptr == before_b_ptr)
            {
                __link(before_a_ptr, b_ptr);
                __link(b_ptr, a_ptr);
                __link(a_ptr, after_b_ptr);
            }
            else if (a_ptr == after_b_ptr)
            {
                __link(before_b_ptr, a_ptr);
                __link(a_ptr, b_ptr);
                __link(b_ptr, after_a_ptr);
            }
            else if (a_ptr != b_ptr)
            {
                __link(before_b_ptr, a_ptr);
                __link(a_ptr, after_b_ptr);
                __link(before_a_ptr, b_ptr);
                __link(b_ptr, after_a_ptr);
            }
        }
        /*!
         * Swaps two ranges of elements
         * @param first1 First element of the first range
         * @param last1 Last element of the first range
         * @param first2 First element of the second range
         * @param last2 Last element of the second range
         * @note Ranges must not intersect
         */
        template <class T>
        static void swap_ranges(replacement_iterator<T> first1, replacement_iterator<T> last1, replacement_iterator<T> first2, replacement_iterator<T> last2)
        {
            auto before1_ptr = __prev(first1._ptr);
            auto first1_ptr = first1._ptr;
            auto last1_ptr = __prev(last1._ptr);
            auto after1_ptr = last1._ptr;
            auto before2_ptr = __prev(first2._ptr);
            auto first2_ptr = first2._ptr;
            auto last2_ptr = __prev(last2._ptr);
            auto after2_ptr = last2._ptr;

            assert(before1_ptr != nullptr);
            assert(first1_ptr != nullptr);
            assert(last1_ptr != nullptr);
            assert(after1_ptr != nullptr);
            assert(before2_ptr != nullptr);
            assert(first2_ptr != nullptr);
            assert(last2_ptr != nullptr);
            assert(after2_ptr != nullptr);

            //Only possible in looped lists
            assert(first1_ptr != after2_ptr || first2_ptr != after1_ptr);
            if (first2_ptr == after1_ptr)
            {
                __link(before1_ptr, first2_ptr);
                __link(last2_ptr, first1_ptr);
                __link(last1_ptr, after2_ptr);
            }
            else if (first1_ptr == after2_ptr)
            {
                __link(before2_ptr, first1_ptr);
                __link(last1_ptr, first2_ptr);
                __link(last2_ptr, after1_ptr);
            }
            else if (first1_ptr != first2_ptr || last1_ptr != last2_ptr)
            {
                __link(before1_ptr, first2_ptr);
                __link(last2_ptr, after1_ptr);
                __link(before2_ptr, first1_ptr);
                __link(last1_ptr, after2_ptr);
            }
        }

        /*!
         * Reverses the order of elements within range
         * @param first First element of the range
         * @param last Last element of the range
         */
        template <class T>
        static void reverse(replacement_iterator<T> first, replacement_iterator<T> last)
        {
            auto before_ptr = __prev(first._ptr);
            auto first_ptr = first._ptr;
            auto last_ptr = __prev(last._ptr);
            auto after_ptr = last._ptr;

            assert(before_ptr != nullptr);
            assert(first_ptr != nullptr);
            assert(last_ptr != nullptr);
            assert(after_ptr != nullptr);

            auto cur_ptr = __next(first_ptr);
            while(cur_ptr != after_ptr)
            {
                auto next_ptr = __next(cur_ptr);
                __link(cur_ptr, __prev(cur_ptr));
                cur_ptr = next_ptr;
            }
            __link(before_ptr, last_ptr);
            __link(first_ptr, after_ptr);
        }

        /*!
         * Performs a rotation on a range of elements in such a way that the element n_first becomes the first
         * element of the new range and n_first - 1 becomes the last element
         * @param first The beginning of the original range
         * @param n_first The element that should appear at the beginning of the rotated range
         * @param last The end of the original range
         */
        template <class T>
        static void rotate(replacement_iterator<T> first, replacement_iterator<T> n_first, replacement_iterator<T> last)
        {
            move<T>(first, n_first, last);
        }
    };

}

#endif //EXTENDEDASSOCIATIVECONTAINERS_REPLACEMENTITERATOR_H
