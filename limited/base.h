//
// Created on 11/28/19.
//

#ifndef EXTENDEDASSOCIATIVECONTAINERS_LIMITEDCONTAINER_BASE_H
#define EXTENDEDASSOCIATIVECONTAINERS_LIMITEDCONTAINER_BASE_H

#include <stdexcept>
#include "../cache_manager/cache_manager.h"

namespace extended_containers
{
    namespace limited
    {
        /*! An exception that is thrown when an object can not be inserted into a limited container because
         *  object's size plus size of all locked stored objects exceeds container capacity
         */
        class no_space_error : std::runtime_error
        {
        public:
            no_space_error() : std::runtime_error("Can't allocate enough free space to store the object") {}
        };

        template <class T, bool IsConst>
        using __add_const_t = std::conditional_t<IsConst, std::add_const_t<T>, T>;

        /*!
         * Wrapper over iterators pointing to limited container node
         * @tparam ForwardIt Iterator type
         * @tparam ConstForwardIt Const iterator type
         * @tparam IsConst If true iterator will behave as constant iterator
         */
        template<class ForwardIt, class ConstForwardIt = ForwardIt, bool IsConst = false>
        class wrapped_iterator
        {

            static_assert(std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIt>::iterator_category>);
            static_assert(std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ConstForwardIt>::iterator_category>);

        public:

            typedef std::conditional_t<IsConst, ConstForwardIt, ForwardIt> wrapped_type;

            typedef typename std::iterator_traits<wrapped_type>::difference_type difference_type;
            typedef typename std::iterator_traits<wrapped_type>::value_type::first_type value_type;
            typedef typename std::iterator_traits<wrapped_type>::value_type::first_type* pointer;
            typedef typename std::iterator_traits<wrapped_type>::value_type::first_type& reference;
            typedef std::forward_iterator_tag iterator_category;

            /*!
             * Constructor
             */
            wrapped_iterator() = default;

            /*!
             * Copy constructor
             */
            wrapped_iterator(const wrapped_iterator &other) = default;

            /*!
             * Copy assignment
             */
            wrapped_iterator &operator=(const wrapped_iterator &other) = default;

            /*!
             * Wrapping constructor
             * @param iter Iterator to wrap
             */
            wrapped_iterator(wrapped_type iter) : _iterator(iter) {}

            /*!
             * Convert non const iterator to const iterator
             */
            operator wrapped_iterator<ForwardIt, ConstForwardIt, true>() { return {_iterator}; };

            /*!
             * Swap this iterator state with other
             * @param other Other iterator
             */
            void swap(wrapped_iterator &other)
            { std::swap(_iterator, other._iterator); }

            /*!
             * Dereference value stored by iterator
             * @return Value stored by iterator
             */
            reference operator*() const
            {
                return _iterator->first;
            }

            /*!
             * Dereference value stored by iterator
             * @return Pointed to value stored by iterator
             */
            pointer operator->() const
            {
                return &_iterator->first;
            }

            /*!
             * Prefix increment operator
             * @return This iterator after increment
             */
            wrapped_iterator &operator++()
            {
                ++_iterator;
                return *this;
            }

            /*!
             * Postfix increment operator
             * @return replacement_iterator pointing to same value as this iterator before increment
             */
            wrapped_iterator operator++(int)
            {
                auto tmp = *this;
                ++_iterator;
                return tmp;
            }

            /*!
             * Compare this iterator with other iterator
             * @param other Other iterator
             * @return True if both iterators point to same object, false otherwise
             */
            template <bool isConst1>
            bool operator==(const wrapped_iterator<ForwardIt, ConstForwardIt, isConst1> &other) const
            { return _iterator == other._iterator; }

            /*!
             * Compare this iterator with other iterator
             * @param other Other iterator
             * @return False if both iterators point to same object, true otherwise
             */
            template <bool isConst1>
            bool operator!=(const wrapped_iterator<ForwardIt, ConstForwardIt, isConst1> &other) const
            { return _iterator != other._iterator; }

        private:

            wrapped_type _iterator;

            template<class ForwardIt1, class ConstForwardIt1, bool IsConst1>
            friend class wrapped_iterator;

            template<
                    class Key1,
                    class T1,
                    class Hash,
                    class KeyEqual,
                    class Weigher,
                    class RepPolicy,
                    class Allocator
            > friend class unordered_map;

        };

        /*!
         * Swap two iterator states
         * @param it1 First iterator
         * @param it2 Second iterator
         */
        template<class ForwardIt, class ConstForwardIt, bool IsConst>
        void swap(wrapped_iterator<ForwardIt, ConstForwardIt, IsConst> &it1,
                  wrapped_iterator<ForwardIt, ConstForwardIt, IsConst> &it2)
        { it1.swap(it2); }

        /*!
         * Class to wrap extracted limited set nodes
         * @tparam Node Type of limited set node
         */
        template <class Node>
        class key_node_wrapper
        {

            static_assert(std::is_nothrow_default_constructible_v<Node>);
            static_assert(std::is_nothrow_move_constructible_v<Node>);
            static_assert(std::is_nothrow_move_assignable_v<Node>);
            static_assert(std::is_nothrow_swappable_v<Node>);

        public:

            typedef typename Node::key_type value_type;
            typedef typename Node::allocator_type allocator_type;

            constexpr key_node_wrapper() noexcept = default;
            key_node_wrapper(key_node_wrapper&&) noexcept = default;
            key_node_wrapper(Node&& node) noexcept : _node(std::move(node)) {}
            key_node_wrapper& operator=(key_node_wrapper&&) noexcept = default;
            key_node_wrapper& operator=(Node&& node) noexcept
            {
                _node = std::move(node);
                return *this;
            }

            bool empty() const noexcept
            {
                return _node.empty();
            }

            explicit operator bool() const noexcept
            {
                return bool(_node);
            }

            allocator_type get_allocator() const
            {
                return _node.get_allocator();
            }

            value_type& value() const
            {
                return _node.key();
            }

            void swap(key_node_wrapper& other) noexcept
            {
                _node.swap(other._node);
            }

        private:

            Node _node;

        };

        template <class Node>
        void swap(key_node_wrapper<Node>& x, key_node_wrapper<Node>& y)
        {
            x.swap(y);
        }

        /*!
         * Class to wrap extracted limited map nodes
         * @tparam Node Type of limited map node
         */
        template <class Node>
        class key_value_node_wrapper
        {

            static_assert(std::is_nothrow_default_constructible_v<Node>);
            static_assert(std::is_nothrow_move_constructible_v<Node>);
            static_assert(std::is_nothrow_move_assignable_v<Node>);
            static_assert(std::is_nothrow_swappable_v<Node>);

        public:

            typedef typename Node::key_type::first_type key_type;
            typedef typename Node::key_type::second_type mapped_type;
            typedef typename Node::allocator_type allocator_type;

            constexpr key_value_node_wrapper() noexcept = default;
            key_value_node_wrapper(key_value_node_wrapper&&) noexcept = default;
            key_value_node_wrapper(Node&& node) noexcept : _node(std::move(node)) {}
            key_value_node_wrapper& operator=(key_value_node_wrapper&&) noexcept = default;
            key_value_node_wrapper& operator=(Node&& node) noexcept
            {
                _node = std::move(node);
                return *this;
            }

            bool empty() const noexcept
            {
                return _node.empty();
            }

            explicit operator bool() const noexcept
            {
                return bool(_node);
            }

            allocator_type get_allocator() const
            {
                return _node.get_allocator();
            }

            key_type & key() const
            {
                return _node.key().first;
            }

            mapped_type & mapped() const
            {
                return _node.key().second;
            }

            void swap(key_value_node_wrapper& other) noexcept
            {
                _node.swap(other._node);
            }

        private:

            Node _node;
            template<
                    class Key1,
                    class T1,
                    class Hash,
                    class KeyEqual,
                    class Weigher,
                    class RepPolicy,
                    class Allocator
            > friend class unordered_map;

        };

        template <class Node>
        void swap(key_value_node_wrapper<Node>& x, key_value_node_wrapper<Node>& y)
        {
            x.swap(y);
        }

        /*!
         * Type that describes return of inserting node into a limited set methods
         * @tparam InsertReturnType wrapped type
         */
        template<class InsertReturnType>
        struct set_insert_return_type_wrapper
        {
            wrapped_iterator<decltype(InsertReturnType::position)> position;
            decltype(InsertReturnType::inserted) inserted;
            key_node_wrapper<decltype(InsertReturnType::node)> node;
            set_insert_return_type_wrapper(InsertReturnType&& val)
                : position(val.position), inserted(val.inserted), node(std::move(val.node)) {}
        };

        /*!
         * Type that describes return of inserting node into a limited map methods
         * @tparam InsertReturnType wrapped type
         */
        template<class InsertReturnType>
        struct map_insert_return_type_wrapper
        {
            wrapped_iterator<decltype(InsertReturnType::position)> position;
            decltype(InsertReturnType::inserted) inserted;
            key_value_node_wrapper<decltype(InsertReturnType::node)> node;
            map_insert_return_type_wrapper(InsertReturnType&& val)
                    : position(val.position), inserted(val.inserted), node(std::move(val.node)) {}
        };

    }
}

#endif //EXTENDEDASSOCIATIVECONTAINERS_LIMITEDCONTAINER_BASE_H
