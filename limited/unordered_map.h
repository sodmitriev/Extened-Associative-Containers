// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef EXTENDEDASSOCIATIVECONTAINERS_UNORDERED_MAP_H
#define EXTENDEDASSOCIATIVECONTAINERS_UNORDERED_MAP_H

#include "base.h"
#include <vector>
#include <deque>
#include <unordered_map>

namespace extended_containers
{
    namespace limited
    {

        /**
          *  @brief A container composed of unique keys (containing
          *  at most one of each key value) that associates values of another type
          *  with the keys and manages contained values caching and replacement.
          *
          *  @ingroup unordered_associative_containers
          *
          *  @tparam  Key        Type of key objects.
          *  @tparam  T          Type of mapped objects.
          *  @tparam  Hash       Hashing function object type, defaults to hash<Value>.
          *  @tparam  KeyEqual   Predicate function object type, defaults to equal_to<Value>.
          *  @tparam  Weigher       Size calculation function object type, defaults to cache_manager::Size<std::pair<const Key, T>>.
          *  @tparam  RepPolicy  Replacement policy type, defaults to cache_manager::Policy::FIFO<std::pair<const Key, T>>.
          *  @tparam  Allocator  Allocator type, defaults to std::allocator<std::pair<const Key, T>>.
          *
          *  Meets the requirements of a <a href="tables.html#65">container</a>, and
          *  <a href="tables.html#xx">unordered associative container</a>
          *
          * The resulting value type of the container is std::pair<const Key, T>.
          */
        template<
                class Key,
                class T,
                class Hash = std::hash<Key>,
                class KeyEqual = std::equal_to<Key>,
                class Weigher = cache_manager::weight<std::pair<const Key, T>>,
                class RepPolicy = cache_manager::policy::lru<std::pair<const Key, T>>,
                class Allocator = std::allocator<cache_manager::stored_node<std::pair<Key, T>>>
        > class unordered_map
        {

            class _hash_wrapper
            {
                Hash _hash;
            public:
                _hash_wrapper() = default;
                _hash_wrapper(const _hash_wrapper&) noexcept(std::is_nothrow_copy_constructible_v<Hash>) = default;
                _hash_wrapper(_hash_wrapper&&) noexcept(std::is_nothrow_move_constructible_v<Hash>) = default;
                _hash_wrapper(const Hash& hash) noexcept(std::is_nothrow_copy_constructible_v<Hash>) : _hash(hash) {}
                _hash_wrapper(Hash&& hash) noexcept(std::is_nothrow_move_constructible_v<Hash>) : _hash(std::move(hash)) {}

                _hash_wrapper& operator=(const _hash_wrapper&) noexcept(std::is_nothrow_copy_assignable_v<Hash>) = default;
                _hash_wrapper& operator=(_hash_wrapper&&) noexcept(std::is_nothrow_move_assignable_v<Hash>) = default;

                 auto operator()(const std::pair<Key, T>& val) const
                 {
                     return _hash(val.first);
                 }

                 const Hash& get() const
                 {
                     return _hash;
                 }
            };

            class _key_equal_wrapper
            {
                KeyEqual _key_equal;
            public:
                _key_equal_wrapper() = default;
                _key_equal_wrapper(const _key_equal_wrapper&) noexcept(std::is_nothrow_copy_constructible_v<KeyEqual>) = default;
                _key_equal_wrapper(_key_equal_wrapper&&) noexcept(std::is_nothrow_move_constructible_v<KeyEqual>) = default;
                _key_equal_wrapper(const KeyEqual& key_equal) noexcept(std::is_nothrow_copy_constructible_v<KeyEqual>) : _key_equal(key_equal) {}
                _key_equal_wrapper(KeyEqual&& key_equal) noexcept(std::is_nothrow_move_constructible_v<KeyEqual>) : _key_equal(std::move(key_equal)) {}

                _key_equal_wrapper& operator=(const _key_equal_wrapper&) noexcept(std::is_nothrow_copy_assignable_v<KeyEqual>) = default;
                _key_equal_wrapper& operator=(_key_equal_wrapper&&) noexcept(std::is_nothrow_move_assignable_v<KeyEqual>) = default;

                auto operator()(const std::pair<Key, T>& lhs, const std::pair<Key, T>& rhs) const
                {
                    return _key_equal(lhs.first, rhs.first);
                }

                const KeyEqual& get() const
                {
                    return _key_equal;
                }
            };

            std::unordered_map<std::pair<Key, T>, cache_manager::cache_node<std::pair<Key, T>>,
                               _hash_wrapper, _key_equal_wrapper, Allocator> _map;
            cache_manager::cache_manager<std::pair<Key, T>, Weigher, RepPolicy> _manager;

        public:

            typedef typename decltype(_map)::key_type::first_type key_type;
            typedef typename decltype(_map)::key_type  value_type;
            typedef typename decltype(_map)::key_type::second_type mapped_type;
            typedef Hash hasher;
            typedef KeyEqual key_equal;
            typedef typename decltype(_manager)::weigher_type weigher_type;
            typedef typename decltype(_manager)::policy_type policy_type;
            typedef typename decltype(_map)::allocator_type allocator_type;
            typedef typename decltype(_manager)::weight_type weight_type;

            typedef typename decltype(_map)::pointer pointer;
            typedef typename decltype(_map)::const_pointer const_pointer;
            typedef value_type&	reference;
            typedef const value_type& const_reference;
            typedef wrapped_iterator<typename decltype(_map)::iterator, typename decltype(_map)::const_iterator, false> iterator;
            typedef wrapped_iterator<typename decltype(_map)::iterator, typename decltype(_map)::const_iterator, true> const_iterator;
            typedef wrapped_iterator<typename decltype(_map)::local_iterator, typename decltype(_map)::const_local_iterator, false> local_iterator;
            typedef wrapped_iterator<typename decltype(_map)::local_iterator, typename decltype(_map)::const_local_iterator, true>	const_local_iterator;
            typedef typename decltype(_manager)::iterator replacement_iterator;
            typedef typename decltype(_manager)::const_iterator const_replacement_iterator;
            typedef typename decltype(_map)::size_type size_type;
            typedef typename decltype(_map)::difference_type difference_type;

            using node_type = key_value_node_wrapper<typename decltype(_map)::node_type>;
            using insert_return_type = map_insert_return_type_wrapper<typename decltype(_map)::insert_return_type>;


            //construct/destroy/copy

            /// Default constructor.
            unordered_map() = default;

            /**
             *  @brief  Default constructor creates no elements.
             *  @param capacity Cache capacity.
             *  @param bucket_count  Minimal initial number of buckets.
             *  @param hash  A hash functor.
             *  @param equal  A key equality functor.
             *  @param weigher A size functor.
             *  @param policy A policy object.
             *  @param alloc  An allocator object.
             */
            explicit
            unordered_map(weight_type capacity,
                          size_type bucket_count = 1,
                          const hasher& hash = hasher(),
                          const key_equal& equal = key_equal(),
                          const weigher_type& weigher = weigher_type(),
                          const policy_type& policy = policy_type(),
                          const allocator_type& alloc = allocator_type())
                    : _map(bucket_count, hash, equal, alloc), _manager(capacity, weigher, policy)
            {}

            /**
             *  @brief  Builds an %unordered_map from a range.
             *  @param  first  An input iterator.
             *  @param  last  An input iterator.
             *  @param  capacity Cache capacity.
             *  @param  bucket_count  Minimal initial number of buckets.
             *  @param  hash  A hash functor.
             *  @param  equal  A key equality functor.
             *  @param  weigher A size functor.
             *  @param  policy A policy object.
             *  @param  alloc  An allocator object.
             *
             *  Create an %unordered_map consisting of copies of the elements from
             *  [first,last).  This is linear in N (where N is
             *  distance(first,last)).
             */
            template<typename InputIterator>
            unordered_map(InputIterator first, InputIterator last,
                          weight_type capacity,
                          size_type bucket_count = 0,
                          const hasher& hash = hasher(),
                          const key_equal& equal = key_equal(),
                          const weigher_type& weigher = weigher_type(),
                          const policy_type& policy = policy_type(),
                          const allocator_type& alloc = allocator_type())
                    : _map(bucket_count, hash, equal, alloc), _manager(capacity, weigher, policy)
            {
                insert(first, last);
            }

            /// Copy constructor.
            unordered_map(const unordered_map& other)
                : unordered_map(other.begin(), other.end(), other._manager.capacity(),
                        other.bucket_count(), other.hash_function(), other.key_eq(),
                        other.weigher(), other.policy(), other.get_allocator())
            { }

            /// Move constructor.
            unordered_map(unordered_map&&) = default;

            /**
             *  @brief Creates an %unordered_map with no elements.
             *  @param capacity Cache capacity.
             *  @param alloc An allocator object.
             */
            explicit
            unordered_map(weight_type capacity,
                          const allocator_type& alloc)
                    : _map(alloc), _manager(capacity, weigher_type(), policy_type())
            { }

            explicit
            unordered_map(const allocator_type& alloc)
                    : unordered_map(0, alloc)
            { }

            /*
             *  @brief Copy constructor with allocator argument.
             *  @param umap  Input %unordered_map to copy.
             *  @param alloc  An allocator object.
             */
            unordered_map(const unordered_map& umap,
                          const allocator_type& alloc)
                    : _map(alloc), _manager(umap.capacity(), umap.weigher(), umap.policy())
            {
                insert(umap.begin(), umap.end());
            }

            /*
             *  @brief  Move constructor with allocator argument.
             *  @param  umap Input %unordered_map to move.
             *  @param  alloc    An allocator object.
             */
            unordered_map(unordered_map&& umap,
                          const allocator_type& alloc)
                    : _map(std::move(umap._map), alloc), _manager(std::move(umap._manager))
            {  }

            /**
             *  @brief  Builds an %unordered_map from an initializer_list.
             *  @param  list  An initializer_list.
             *  @param  capacity Cache capacity.
             *  @param  bucket_count  Minimal initial number of buckets.
             *  @param  hash  A hash functor.
             *  @param  equal  A key equality functor.
             *  @param  weigher A size functor.
             *  @param  policy A policy object.
             *  @param  alloc  An allocator object.
             *
             *  Create an %unordered_map consisting of copies of the elements in the
             *  list. This is linear in N (where N is @a list.size()).
             */
            unordered_map(std::initializer_list<value_type> list,
                          weight_type capacity,
                          size_type bucket_count = 0,
                          const hasher& hash = hasher(),
                          const key_equal& equal = key_equal(),
                          const weigher_type& weigher = weigher_type(),
                          const policy_type& policy = policy_type(),
                          const allocator_type& alloc = allocator_type())
                    : unordered_map(list.begin(), list.end(), capacity, bucket_count, hash, equal, weigher, policy, alloc)
            {  }

            unordered_map(weight_type capacity, size_type bucket_count, const allocator_type& alloc)
                    : unordered_map(capacity, bucket_count, hasher(), key_equal(), weigher_type(), policy_type(), alloc)
            { }

            unordered_map(weight_type capacity, size_type bucket_count, const hasher& hash,
                          const allocator_type& alloc)
                    : unordered_map(capacity, bucket_count, hash, key_equal(), weigher_type(), policy_type(), alloc)
            { }

            unordered_map(weight_type capacity, size_type bucket_count, const hasher& hash, const key_equal& equal,
                          const allocator_type& alloc)
                    : unordered_map(capacity, bucket_count, hash, equal, weigher_type(), policy_type(), alloc)
            { }

            unordered_map(weight_type capacity, size_type bucket_count, const hasher& hash, const key_equal& equal,
                          const weigher_type& weigher, const allocator_type& alloc)
                    : unordered_map(capacity, bucket_count, hash, equal, weigher, policy_type(), alloc)
            { }

            template<typename InputIterator>
            unordered_map(InputIterator first, InputIterator last,
                          weight_type capacity, size_type bucket_count,
                          const allocator_type& alloc)
                    : unordered_map(first, last, capacity, bucket_count, hasher(), key_equal(), weigher_type(),
                            policy_type(), alloc)
            { }

            template<typename InputIterator>
            unordered_map(InputIterator first, InputIterator last,
                          weight_type capacity, size_type bucket_count,
                          const hasher& hash,
                          const allocator_type& alloc)
                    : unordered_map(first, last, capacity, bucket_count, hash, key_equal(), weigher_type(),
                                    policy_type(), alloc)
            { }

            template<typename InputIterator>
            unordered_map(InputIterator first, InputIterator last,
                          weight_type capacity, size_type bucket_count,
                          const hasher& hash, const key_equal& equal,
                          const allocator_type& alloc)
                    : unordered_map(first, last, capacity, bucket_count, hash, equal, weigher_type(),
                                    policy_type(), alloc)
            { }

            template<typename InputIterator>
            unordered_map(InputIterator first, InputIterator last,
                          weight_type capacity, size_type bucket_count,
                          const hasher& hash, const key_equal& equal, const weigher_type& weigher,
                          const allocator_type& alloc)
                    : unordered_map(first, last, capacity, bucket_count, hash, equal, weigher,
                                    policy_type(), alloc)
            { }

            unordered_map(std::initializer_list<value_type> list,
                          weight_type capacity, size_type bucket_count,
                          const allocator_type& alloc)
                    : unordered_map(list, capacity, bucket_count, hasher(), key_equal(), weigher_type(),
                                    policy_type(), alloc)
            { }

            unordered_map(std::initializer_list<value_type> list,
                          weight_type capacity, size_type bucket_count,
                          const hasher& hash,
                          const allocator_type& alloc)
                    : unordered_map(list, capacity, bucket_count, hash, key_equal(), weigher_type(),
                                    policy_type(), alloc)
            { }

            unordered_map(std::initializer_list<value_type> list,
                          weight_type capacity, size_type bucket_count,
                          const hasher& hash, const key_equal& equal,
                          const allocator_type& alloc)
                    : unordered_map(list, capacity, bucket_count, hash, equal, weigher_type(),
                                    policy_type(), alloc)
            { }

            unordered_map(std::initializer_list<value_type> list,
                          weight_type capacity, size_type bucket_count,
                          const hasher& hash, const key_equal& equal, const weigher_type& weigher,
                          const allocator_type& alloc)
                    : unordered_map(list, capacity, bucket_count, hash, equal, weigher,
                                    policy_type(), alloc)
            { }

            /// Copy assignment operator.
            unordered_map&
            operator=(const unordered_map& other)
            {
                _manager = decltype(_manager)(other.capacity(), other.weigher(), other.policy());
                _map.clear();
                _map.reserve(other._map.size());
#ifndef NDEBUG
                try
                {
                    insert(other.begin(), other.end());
                }
                catch(no_space_error&)
                {
                    assert("Not enough space in copied container" == nullptr);
                }
#else
                insert(other.begin(), other.end());
#endif
                return *this;
            }

            /// Move assignment operator.
            unordered_map&
            operator=(unordered_map&& other) = default;

            /**
             *  @brief  %Unordered_map list assignment operator.
             *  @param  list  An initializer_list.
             *  @throws no_space_error Weight of elements in list exceeds capacity (container unaltered if thrown)
             *
             *  This function fills an %unordered_map with copies of the elements in
             *  the initializer list @a list.
             *
             *  Note that the assignment completely changes the %unordered_map and
             *  that the resulting %unordered_map's size is the same as the number
             *  of elements assigned.
             */
            unordered_map&
            operator=(std::initializer_list<value_type> list)
            {
                size_t weight = 0;
                for(auto &i : list)
                {
                    weight += _manager.calculate_weight(i);
                }
                if(weight > _manager.capacity())
                {
                    throw no_space_error();
                }
                _manager.clear();
                _map.clear();
                _map.reserve(list.size());
#ifndef NDEBUG
                try
                {
                    insert(list.begin(), list.end());
                }
                catch(no_space_error&)
                {
                    assert("Space is was checked before but insertion still failed" == nullptr);
                }
#else
                insert(list.begin(), list.end());
#endif
                return *this;
            }

            ///  Returns the allocator object used by the %unordered_map.
            allocator_type
            get_allocator() const noexcept
            { return _map.get_allocator(); }

            // size and capacity:

            ///  Returns true if the %unordered_map is empty.
            [[nodiscard]]
            bool empty() const noexcept
            {
                return _map.empty();
            }

            ///  Returns the size of the %unordered_map.
            size_type
            size() const noexcept
            {
                return _map.size();
            }

            ///  Returns the maximum size of the %unordered_map.
            size_type
            max_size() const noexcept
            { return _map.max_size(); }

            ///  Returns the current total weight of elements in the %unordered_map.
            weight_type
            weight() const noexcept
            { return _manager.weight(); }

            ///  Returns the maximum total weight of elements in the %unordered_map.
            weight_type
            capacity() const noexcept
            { return _manager.capacity(); }

            /**
             * Checks if object with specified size can fit into the container
             * @param weight Weight of the object
             * @return True if object can fit, false otherwise
             */
            bool can_fit(weight_type weight) const
            {
                return _manager.can_fit(weight);
            }

            /**
             * Checks if provided object can fit into the container
             * @param weight Object to check
             * @return True if object can fit, false otherwise
             */
            bool can_fit(const value_type& obj) const
            {
                return _manager.can_fit(obj);
            }

            /**
             * Tries to free the container to provide required amount of free space
             * @param space Required free space
             * @throw Containers::limited::NoSpace If required space can not be provided
             */
            void provide_space(weight_type space)
            {
                if(!provideSpace(space))
                {
                    throw no_space_error();
                }
            }

            /**
             * Tries to free the container to provide amount of free space that is
             * required to store specified object
             * @param obj Object to fit
             * @throw Containers::limited::NoSpace If required space can not be provided
             */
            void provide_space(const value_type& obj)
            {
                provide_space(_manager.calculate_weight(obj));
            }

            /**
             * Tries to free the specified amount of space from container
             * @param space Required free space
             * @throw Containers::limited::NoSpace If required space can not be provided
             */
            void free_space(weight_type space)
            {
                if(!freeSpace(space))
                {
                    throw no_space_error();
                }
            }

            /**
             * Moves an element in the replacement queue the same way as if it has been accessed
             * @param key Key of an element
             */
            void access(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if(it != _map.end())
                {
                    _manager.access(&*it);
                }
            }

            /**
             * Moves an element in the replacement queue the same way as if it has been accessed
             * @param pos replacement_iterator pointing to the element
             */
            void access(iterator pos)
            {
                _manager.access(&*pos._iterator);
            }

            /**
             * Moves an element in the replacement queue the same way as if it has been accessed
             * @param pos Replacement iterator pointing to the element
             */
            void access(replacement_iterator pos)
            {
                _manager.access(pos);
            }

            // iterators.

            /**
             *  Returns a read/write iterator that points to the first element in the
             *  %unordered_map.
             */
            iterator
            begin() noexcept
            { return _map.begin(); }

            //@{
            /**
             *  Returns a read-only (constant) iterator that points to the first
             *  element in the %unordered_map.
             */
            const_iterator
            begin() const noexcept
            { return _map.begin(); }

            const_iterator
            cbegin() const noexcept
            { return _map.begin(); }
            //@}

            /**
             *  Returns a read/write iterator that points one past the last element in
             *  the %unordered_map.
             */
            iterator
            end() noexcept
            { return _map.end(); }

            //@{
            /**
             *  Returns a read-only (constant) iterator that points one past the last
             *  element in the %unordered_map.
             */
            const_iterator
            end() const noexcept
            { return _map.end(); }

            const_iterator
            cend() const noexcept
            { return _map.end(); }
            //@}

            /**
             *  Returns an iterator that points to the first element in replacement order list and
             *  can be used to modify the list.
             */
            replacement_iterator
            replacement_begin() noexcept
            { return _manager.begin(); }

            //@{
            /**
             *  Returns an iterator that points to the first element in replacement order list.
             */
            const_replacement_iterator
            replacement_begin() const noexcept
            { return _manager.begin(); }

            const_replacement_iterator
            replacement_cbegin() const noexcept
            { return _manager.begin(); }
            //@}

            /**
             *  Returns an iterator that points one past the last element in replacement order list and
             *  can be used to modify the list.
             */
            replacement_iterator
            replacement_end() noexcept
            { return _manager.end(); }

            //@{
            /**
             *  Returns an iterator that points one past the last element in replacement order list.
             */
            const_replacement_iterator
            replacement_end() const noexcept
            { return _manager.end(); }

            const_replacement_iterator
            replacement_cend() const noexcept
            { return _manager.end(); }
            //@}

            // modifiers.

            /**
             *  @brief Attempts to build and insert a std::pair into the
             *  %unordered_map.
             *
             *  @param args  Arguments used to generate a new pair instance (see
             *	        std::piecewise_contruct for passing arguments to each
             *	        part of the pair constructor).
             *
             *  @return  A pair, of which the first element is an iterator that points
             *           to the possibly inserted pair, and the second is a bool that
             *           is true if the pair was actually inserted.
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  This function attempts to build and insert a (key, value) %pair into
             *  the %unordered_map.
             *  An %unordered_map relies on unique keys and thus a %pair is only
             *  inserted if its first element (the key) is not already present in the
             *  %unordered_map.
             *
             *  Insertion requires amortized constant time.
             */
            template<typename... Args>
            std::pair<iterator, bool>
            emplace(Args&&... args)
            {
                auto it = _map.emplace(std::piecewise_construct, std::tuple{std::forward<Args>(args)...}, std::tuple{});
                if(it.second)
                {
                    indexInserted(it.first);
                }
                return it;
            }

            /**
             *  @brief Attempts to build and insert a std::pair into the
             *  %unordered_map.
             *
             *  @param  pos  An iterator that serves as a hint as to where the pair
             *                should be inserted.
             *  @param  args  Arguments used to generate a new pair instance (see
             *	         std::piecewise_contruct for passing arguments to each
             *	         part of the pair constructor).
             *  @return An iterator that points to the element with key of the
             *          std::pair built from @a args (may or may not be that
             *          std::pair).
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  This function is not concerned about whether the insertion took place,
             *  and thus does not return a boolean like the single-argument emplace()
             *  does.
             *  Note that the first parameter is only a hint and can potentially
             *  improve the performance of the insertion process. A bad hint would
             *  cause no gains in efficiency.
             *
             *  See
             *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
             *  for more on @a hinting.
             *
             *  Insertion requires amortized constant time.
             */
            template<typename... Args>
            iterator
            emplace_hint(const_iterator, Args&&... args)
            {
                //Can't efficiently implement insertions with hint
                return emplace(std::forward<Args>(args)...).first;
            }

            /// Extract a node.
            node_type
            extract(const_iterator pos)
            {
                _manager.erase(&*pos._iterator);
                return _map.extract(pos._iterator);
            }

            /// Extract a node.
            node_type
            extract(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if(it != _map.end())
                {
                    _manager.erase(&*it);
                    return _map.extract(it);
                }
                return {};
            }

            /// Re-insert an extracted node.
            insert_return_type
            insert(node_type&& nh)
            {
                auto it = _map.insert(std::move(nh._node));
                if(it.inserted)
                {
                    indexInserted(it.position);
                }
                return it;
            }

            /// Re-insert an extracted node.
            iterator
            insert(const_iterator, node_type&& nh)
            {
                //Can't efficiently implement insertions with hint
                return insert(std::move(nh._node)).position._iterator;
            }

            /**
             *  @brief Attempts to build and insert a std::pair into the
             *  %unordered_map.
             *
             *  @param k    Key to use for finding a possibly existing pair in
             *                the unordered_map.
             *  @param args  Arguments used to generate the .second for a
             *                new pair instance.
             *
             *  @return  A pair, of which the first element is an iterator that points
             *           to the possibly inserted pair, and the second is a bool that
             *           is true if the pair was actually inserted.
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  This function attempts to build and insert a (key, value) %pair into
             *  the %unordered_map.
             *  An %unordered_map relies on unique keys and thus a %pair is only
             *  inserted if its first element (the key) is not already present in the
             *  %unordered_map.
             *  If a %pair is not inserted, this function has no effect.
             *
             *  Insertion requires amortized constant time.
             */
            template <typename... Args>
            std::pair<iterator, bool>
            try_emplace(const key_type& k, Args&&... args)
            {
                //Impossible to efficiently implement
                return emplace(std::piecewise_construct, std::tuple{k}, std::tuple{std::forward<Args>(args)...});
            }

            // move-capable overload
            template <typename... Args>
            std::pair<iterator, bool>
            try_emplace(key_type&& k, Args&&... args)
            {
                //Impossible to efficiently implement
                return emplace(std::piecewise_construct, std::tuple{std::move(k)}, std::tuple{std::forward<Args>(args)...});
            }

            /**
             *  @brief Attempts to build and insert a std::pair into the
             *  %unordered_map.
             *
             *  @param  hint  An iterator that serves as a hint as to where the pair
             *                should be inserted.
             *  @param k    Key to use for finding a possibly existing pair in
             *                the unordered_map.
             *  @param args  Arguments used to generate the .second for a
             *                new pair instance.
             *  @return An iterator that points to the element with key of the
             *          std::pair built from @a args (may or may not be that
             *          std::pair).
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  This function is not concerned about whether the insertion took place,
             *  and thus does not return a boolean like the single-argument emplace()
             *  does. However, if insertion did not take place,
             *  this function has no effect.
             *  Note that the first parameter is only a hint and can potentially
             *  improve the performance of the insertion process. A bad hint would
             *  cause no gains in efficiency.
             *
             *  See
             *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
             *  for more on @a hinting.
             *
             *  Insertion requires amortized constant time.
             */
            template <typename... Args>
            iterator
            try_emplace(const_iterator, const key_type& k,
                        Args&&... args)
            {
                //Can't efficiently implement insertions with hint
                return try_emplace(k, std::forward<Args>(args)...).first;
            }

            // move-capable overload
            template <typename... Args>
            iterator
            try_emplace(const_iterator, key_type&& k, Args&&... args)
            {
                //Can't efficiently implement insertions with hint
                return try_emplace(std::move(k), std::forward<Args>(args)...).first;
            }

            //@{
            /**
             *  @brief Attempts to insert a std::pair into the %unordered_map.

             *  @param value Pair to be inserted (see std::make_pair for easy
             *	     creation of pairs).
             *
             *  @return  A pair, of which the first element is an iterator that
             *           points to the possibly inserted pair, and the second is
             *           a bool that is true if the pair was actually inserted.
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  This function attempts to insert a (key, value) %pair into the
             *  %unordered_map. An %unordered_map relies on unique keys and thus a
             *  %pair is only inserted if its first element (the key) is not already
             *  present in the %unordered_map.
             *
             *  Insertion requires amortized constant time.
             */
            std::pair<iterator, bool>
            insert(const value_type& value)
            {
                auto it = _map.insert({value, {}});
                if(it.second)
                {
                    indexInserted(it.first);
                }
                return it;
            }

            std::pair<iterator, bool>
            insert(value_type&& value)
            {
                auto it = _map.insert({std::move(value), {}});
                if(it.second)
                {
                    indexInserted(it.first);
                }
                return it;
            }

            template<typename Pair>
            std::enable_if_t<std::is_constructible<value_type, Pair&&>::value &&
                             !std::is_same<value_type, typename std::remove_reference<Pair>::type>::value,
                             std::pair<iterator, bool>>
            insert(Pair&& value)
            {
                auto it = _map.insert({std::forward<Pair>(value), {}});
                if(it.second)
                {
                    indexInserted(it.first);
                }
                return it;
            }
            //@}

            //@{
            /**
             *  @brief Attempts to insert a std::pair into the %unordered_map.
             *  @param  hint  An iterator that serves as a hint as to where the
             *                 pair should be inserted.
             *  @param  value  Pair to be inserted (see std::make_pair for easy creation
             *               of pairs).
             *  @return An iterator that points to the element with key of
             *           @a value (may or may not be the %pair passed in).
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  This function is not concerned about whether the insertion took place,
             *  and thus does not return a boolean like the single-argument insert()
             *  does.  Note that the first parameter is only a hint and can
             *  potentially improve the performance of the insertion process.  A bad
             *  hint would cause no gains in efficiency.
             *
             *  See
             *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
             *  for more on @a hinting.
             *
             *  Insertion requires amortized constant time.
             */
            iterator
            insert(const_iterator, const value_type& value)
            {
                //Can't efficiently implement insertions with hint
                return insert(value).first;
            }

            iterator
            insert(const_iterator, value_type&& value)
            {
                //Can't efficiently implement insertions with hint
                return insert(std::move(value)).first;
            }

            template<typename Pair>
            std::enable_if_t<std::is_constructible<value_type, Pair&&>::value &&
                             !std::is_same<value_type, typename std::remove_reference<Pair>::type>::value,
                             iterator>
            insert(const_iterator, Pair&& value)
            {
                //Can't efficiently implement insertions with hint
                return insert(std::forward<Pair>(value)).first;
            }
            //@}

            /**
             *  @brief A template function that attempts to insert a range of
             *  elements.
             *  @param  first  replacement_iterator pointing to the start of the range to be
             *                   inserted.
             *  @param  last  replacement_iterator pointing to the end of the range.
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  Complexity similar to that of the range constructor.
             */
            template<typename InputIterator>
            void
            insert(InputIterator first, InputIterator last)
            {
                std::vector<std::add_pointer_t<cache_manager::stored_node<std::pair<int, int>>>> inserted;
                size_t weight = 0;
                try
                {
                    for(; first != last; ++first)
                    {
                        auto ret = _map.insert({*first, {}});
                        if(ret.second)
                        {
                            inserted.push_back(&*ret.first);
                            weight += _manager.calculate_weight(ret.first->first);
                            if(weight > _manager.capacity())
                            {
                                break;
                            }
                        }
                    }
                }
                catch(...)
                {
                    for(auto & i : inserted)
                    {
                        _map.erase(i->first);
                    }
                    throw;
                }
                if(!provideSpace(weight))
                {
                    for(auto & i : inserted)
                    {
                        _map.erase(i->first);
                    }
                    throw no_space_error();
                }
                auto it = inserted.cbegin();
                try
                {
                    for (; it != inserted.cend(); ++it)
                    {
                        _manager.insert(*it);
                    }
                }
                catch(...)
                {
                    for(auto i : inserted)
                    {
                        _manager.erase(&*i);
                    }
                    for(auto & i : inserted)
                    {
#ifndef NDEBUG
                        try
                        {
                            _map.erase(i->first);
                        }
                        catch (...)
                        {
                            assert("Exception is thrown in map erase ?!" == nullptr);
                        }
#else
                        _map.erase(i->first);
#endif
                    }
                    throw;
                }
            }

            /**
             *  @brief Attempts to insert a list of elements into the %unordered_map.
             *  @param  list  A std::initializer_list<value_type> of elements
             *               to be inserted.
             *
             *  @throws no_space_error Not enough available space to store new element
             *
             *  Complexity similar to that of the range constructor.
             */
            void
            insert(std::initializer_list<value_type> list)
            {
                insert(list.begin(), list.end());
            }

            /**
             *  @brief Attempts to insert a std::pair into the %unordered_map.
             *  @param k    Key to use for finding a possibly existing pair in
             *                the map.
             *  @param obj  Argument used to generate the .second for a pair
             *                instance.
             *
             *  @return  A pair, of which the first element is an iterator that
             *           points to the possibly inserted pair, and the second is
             *           a bool that is true if the pair was actually inserted.
             *
             *  @throws no_space_error Not enough available space to store new element
             *          (old element is unaltered if thrown)
             *
             *  This function attempts to insert a (key, value) %pair into the
             *  %unordered_map. An %unordered_map relies on unique keys and thus a
             *  %pair is only inserted if its first element (the key) is not already
             *  present in the %unordered_map.
             *  If the %pair was already in the %unordered_map, the .second of
             *  the %pair is assigned from __obj.
             *
             *  Insertion requires amortized constant time.
             */
            template <typename Obj>
            std::pair<iterator, bool>
            insert_or_assign(const key_type& k, Obj&& obj)
            {
                auto it = _map.find({k, {}});
                if(it == _map.end())
                {
                    auto ret = emplace(k, std::forward<Obj>(obj));
                    assert(ret.second);
                    return ret;
                }
                else
                {
                    return {assign(it, std::forward<Obj>(obj)), false};
                }
            }

            // move-capable overload
            template <typename Obj>
            std::pair<iterator, bool>
            insert_or_assign(key_type&& k, Obj&& obj)
            {
                auto it = _map.find({k, {}});
                if(it == _map.end())
                {
                    auto ret = emplace(std::move(k), std::forward<Obj>(obj));
                    assert(ret.second);
                    return ret;
                }
                else
                {
                    return {assign(it, std::forward<Obj>(obj)), false};
                }
            }

            /**
             * Assigns a new value to the element located on position and recalculates occupied space
             * @param position Element position
             * @param obj New value of the element
             * @return New element position
             * @throws no_space_error Not enough available space to store new element
             *         (old element is unaltered if thrown)
             */
            template <typename Obj>
            iterator assign(iterator position, Obj&& obj)
            {
                key_type newVal {std::forward<Obj>(obj)};
                replacement_iterator repl_it = _manager.toIter(&*position._iterator);
                auto oldWeight = _manager.calculate_weight(repl_it);
                auto node = _map.extract(position._iterator);
                std::swap(newVal, node.key().second);
                auto newWeight = _manager.calculate_weight(node.key());
                if(newWeight > oldWeight)
                {
                    try
                    {
                        if (!provideSpace(newWeight - oldWeight, repl_it))
                        {
                            throw no_space_error();
                        }
                    }
                    catch(...)
                    {
                        std::swap(newVal, node.key().second);
                        auto ins = _map.insert(std::move(node));
                        assert(ins.inserted);
                        throw;
                    }
                }
                auto ins = _map.insert(std::move(node));
                assert(ins.inserted);
                _manager.update_weight(repl_it, oldWeight, newWeight);
                _manager.access(repl_it);
                return ins.position;
            }

            /**
             *  @brief Attempts to insert a std::pair into the %unordered_map.
             *  @param  hint  An iterator that serves as a hint as to where the
             *                  pair should be inserted.
             *  @param k    Key to use for finding a possibly existing pair in
             *                the unordered_map.
             *  @param obj  Argument used to generate the .second for a pair
             *                instance.
             *  @return An iterator that points to the element with key of
             *           @a k (may or may not be the %pair passed in).
             *
             * @throws no_space_error Not enough available space to store new element
             *         (old element is unaltered if thrown)
             *
             *  This function is not concerned about whether the insertion took place,
             *  and thus does not return a boolean like the single-argument insert()
             *  does.
             *  If the %pair was already in the %unordered map, the .second of
             *  the %pair is assigned from __obj.
             *  Note that the first parameter is only a hint and can
             *  potentially improve the performance of the insertion process.  A bad
             *  hint would cause no gains in efficiency.
             *
             *  See
             *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
             *  for more on @a hinting.
             *
             *  Insertion requires amortized constant time.
             */
            template <typename Obj>
            iterator
            insert_or_assign(const_iterator, const key_type& k,
                             Obj&& obj)
            {
                //Can't efficiently implement insertions with hint
                return insert_or_assign(k, std::forward<Obj>(obj)).first;
            }

            // move-capable overload
            template <typename Obj>
            iterator
            insert_or_assign(const_iterator, key_type&& k, Obj&& obj)
            {
                //Can't efficiently implement insertions with hint
                return insert_or_assign(std::move(k), std::forward<Obj>(obj)).first;
            }

            //@{
            /**
             *  @brief Erases an element from an %unordered_map.
             *  @param  position  An iterator pointing to the element to be erased.
             *  @return An iterator pointing to the element immediately following
             *          @a position prior to the element being erased. If no such
             *          element exists, end() is returned.
             *
             *  This function erases an element, pointed to by the given iterator,
             *  from an %unordered_map.
             *  Note that this function only erases the element, and that if the
             *  element is itself a pointer, the pointed-to memory is not touched in
             *  any way.  Managing the pointer is the user's responsibility.
             */
            iterator
            erase(const_iterator position)
            {
                _manager.erase(&*position._iterator);
                return _map.erase(position._iterator);
            }

            replacement_iterator
            erase(const_replacement_iterator position)
            {
                auto key = &*position;
                auto next = _manager.erase(position);
#ifndef NDEBUG
                auto ret =
#endif
                _map.erase(*key);
#ifndef NDEBUG
                assert(ret == 1);
#endif
                return next;
            }
            //@}

            /**
             *  @brief Erases elements according to the provided key.
             *  @param  key  Key of element to be erased.
             *  @return  The number of elements erased.
             *
             *  This function erases all the elements located by the given key from
             *  an %unordered_map. For an %unordered_map the result of this function
             *  can only be 0 (not present) or 1 (present).
             *  Note that this function only erases the element, and that if the
             *  element is itself a pointer, the pointed-to memory is not touched in
             *  any way.  Managing the pointer is the user's responsibility.
             */
            size_type
            erase(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if(it ==  _map.end())
                {
                    return 0;
                }
                _manager.erase(&*it);
                _map.erase(it);
                return 1;
            }

            //@{
            /**
             *  @brief Erases a [__first,__last) range of elements from an
             *  %unordered_map.
             *  @param  first  replacement_iterator pointing to the start of the range to be
             *                  erased.
             *  @param last  replacement_iterator pointing to the end of the range to
             *                be erased.
             *  @return The iterator @a last.
             *
             *  This function erases a sequence of elements from an %unordered_map.
             *  Note that this function only erases the elements, and that if
             *  the element is itself a pointer, the pointed-to memory is not touched
             *  in any way.  Managing the pointer is the user's responsibility.
             */
            iterator
            erase(const_iterator first, const_iterator last)
            {
                auto it = first;
                while(it != last)
                {
                    _manager.erase(&*it._iterator);
                    ++it;
                }
                return _map.erase(first._iterator, last._iterator);
            }

            replacement_iterator
            erase(const_replacement_iterator first, const_replacement_iterator last)
            {
                std::deque<typename decltype(_map)::node_type> nodes;
                for (auto it = first; it != last; ++it)
                {
                    try
                    {
                        nodes.push_back(_map.extract({it->first, {}}));
                    }
                    catch(...)
                    {
                        for(auto & i : nodes)
                        {
#ifndef NDEBUG
                            try
                            {
                                auto ret =
#endif
                                        _map.insert(std::move(i));
#ifndef NDEBUG
                                assert(ret.inserted);
                            }
                            catch(...)
                            {
                                assert("Exception during reinsertion into map" == nullptr);
                            }
#endif
                        }
                    }
                }
                return _manager.erase(first, last);
            }
            //@}

            /**
             *  Erases all elements in an %unordered_map.
             *  Note that this function only erases the elements, and that if the
             *  elements themselves are pointers, the pointed-to memory is not touched
             *  in any way.  Managing the pointer is the user's responsibility.
             */
            void
            clear() noexcept
            { 
                _manager.clear();
                _map.clear();
            }

            /**
             *  @brief  Swaps data with another %unordered_map.
             *  @param  other  An %unordered_map of the same element and allocator
             *  types.
             *
             *  This exchanges the elements between two %unordered_map in constant
             *  time.
             *  Note that the global std::swap() function is specialized such that
             *  std::swap(m1,m2) will feed to this function.
             */
            void
            swap(unordered_map& other)
            noexcept( noexcept(_map.swap(other._map)) )
            { 
                _map.swap(other._map);
                _manager.swap(other._manager);
            }

            // observers.

            ///  Returns the hash functor object with which the %unordered_map was
            ///  constructed.
            hasher
            hash_function() const
            { return _map.hash_function().get(); }

            ///  Returns the key comparison object with which the %unordered_map was
            ///  constructed.
            key_equal
            key_eq() const
            { return _map.key_eq().get(); }

            ///  Returns the weight calculation object with which the %unordered_map was
            ///  constructed.
            weigher_type
            weigher() const
            { return _manager.get_weigher(); }

            ///  Returns the policy object with which the %unordered_map was
            ///  constructed.
            policy_type
            policy() const
            { return _manager.get_policy(); }

            // lookup.

            //@{
            /**
             *  @brief Tries to locate an element in an %unordered_map.
             *  @param  key  Key to be located.
             *  @return  replacement_iterator pointing to sought-after element, or end() if not
             *           found.
             *
             *  This function takes a key and tries to locate the element with which
             *  the key matches.  If successful the function returns an iterator
             *  pointing to the sought after element.  If unsuccessful it returns the
             *  past-the-end ( @c end() ) iterator.
             */
            iterator
            find(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if (it != _map.cend())
                {
                    _manager.access(&*it);
                }
                return it;
            }

            iterator
            quiet_find(const key_type& key)
            {
                return  _map.find({key, {}});
            }

            const_iterator
            quiet_find(const key_type& key) const
            {
                return _map.find({key, {}});
            }

            replacement_iterator
            replacement_find(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if (it != _map.cend())
                {
                    _manager.access(&*it);
                    return decltype(_manager)::toIter(&*it);
                }
                return _manager.end();
            }

            replacement_iterator
            quiet_replacement_find(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if (it != _map.cend())
                {
                    return  decltype(_manager)::toIter(&*it);
                }
                return _manager.end();
            }

            const_replacement_iterator
            quiet_replacement_find(const key_type& key) const
            {
                auto it = _map.find({key, {}});
                if (it != _map.cend())
                {
                    return  decltype(_manager)::toIter(&*it);
                }
                return _manager.cend();
            }
            //@}

            /**
             *  @brief  Finds the number of elements.
             *  @param  key  Key to count.
             *  @return  Number of elements with specified key.
             *
             *  This function only makes sense for %unordered_multimap; for
             *  %unordered_map the result will either be 0 (not present) or 1
             *  (present).
             */
            size_type
            count(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if (it != _map.cend())
                {
                    _manager.access(&*it);
                    return 1;
                }
                return 0;
            }

            size_type
            quiet_count(const key_type& key) const
            { return _map.count({key, {}}); }

            /**
             *  @brief  Finds whether an element with the given key exists.
             *  @param  key  Key of elements to be located.
             *  @return  True if there is any element with the specified key.
             */
            bool
            contains(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if (it != _map.cend())
                {
                    _manager.access(&*it);
                    return true;
                }
                return false;
            }

            bool
            quiet_contains(const key_type& key) const
            { return _map.count({key, {}}) > 0; }

            //@{
            /**
             *  @brief Finds a subsequence matching given key.
             *  @param  key  Key to be located.
             *  @return  Pair of iterators that possibly points to the subsequence
             *           matching given key.
             *
             *  This function probably only makes sense for %unordered_multimap.
             */
            std::pair<iterator, iterator>
            equal_range(const key_type& key)
            {
                auto it = _map.equal_range({key, {}});
                for (auto it1 = it.first; it1 != it.second; ++it1)
                {
                    _manager.access(&*it1);
                }
                return it;
            }

            std::pair<iterator, iterator>
            quiet_equal_range(const key_type& key)
            {
                return _map.equal_range({key, {}});
            }

            std::pair<const_iterator, const_iterator>
            quiet_equal_range(const key_type& key) const
            {
                return _map.equal_range({key, {}});
            }
            //@}

            //@{
            /**
             *  @brief  Subscript ( @c [] ) access to %unordered_map data.
             *  @param  value  The pair of key and data for which data should be retrieved.
             *  @return  A constant reference to the data of the (key,data) %pair.
             *  @throws no_space_error Not enough available space to store new element
             *
             *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
             *  data associated with the key specified in subscript.  If the key does
             *  not exist, a pair with that key and provided value is created and then returned.
             *
             *  Lookup requires constant time.
             */
            const mapped_type&
            operator[](const value_type& value)
            {
                auto it = _map.find(value);
                if(it == _map.cend())
                {
                    auto ret = insert(value);
                    assert(ret.second);
                    it = ret.first._iterator;
                }
                else
                {
                    _manager.access(&*it);
                }
                return it->first.second;
            }

            const mapped_type&
            operator[](value_type&& value)
            {
                auto it = _map.find(value);
                if(it == _map.cend())
                {
                    auto ret = insert(std::move(value));
                    assert(ret.second);
                    it = ret.first._iterator;
                }
                else
                {
                    _manager.access(&*it);
                }
                return it->first.second;
            }
            //@}

            //@{
            /**
             *  @brief  Subscript ( @c [] ) access to %unordered_map data.
             *  @param  key  The key for which data should be retrieved.
             *  @return  A constant reference to the data of the (key,data) %pair.
             *  @throws no_space_error Not enough available space to store new element
             *          (old element is unaltered if thrown)
             *
             *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
             *  data associated with the key specified in subscript.  If the key does
             *  not exist, a pair with that key is created using default values, which
             *  is then returned.
             *
             *  Lookup requires constant time.
             */
            const mapped_type&
            operator[](const key_type& key)
            {
                return operator[]({key, {}});
            }

            const mapped_type&
            operator[](key_type&& key)
            {
                return operator[]({std::move(key), {}});
            }
            //@}

            //@{
            /**
             *  @brief  Access to %unordered_map data.
             *  @param  key  The key for which data should be retrieved.
             *  @return  A reference to the data whose key is equal to @a key, if
             *           such a data is present in the %unordered_map.
             *  @throw  std::out_of_range  If no such data is present.
             */
            const mapped_type&
            at(const key_type& key)
            {
                auto it = _map.find({key, {}});
                if(it == _map.end())
                {
                    throw std::out_of_range("Data with provided key does not exist");
                }
                _manager.access(&*it);
                return it->first.second;
            }

            const mapped_type&
            quiet_at(const key_type& key) const
            {
                auto it = _map.find({key, {}});
                if(it == _map.end())
                {
                    throw std::out_of_range("Data with provided key does not exist");
                }
                return it->first.second;
            }
            //@}

            // bucket interface.

            /// Returns the number of buckets of the %unordered_map.
            size_type
            bucket_count() const noexcept
            { return _map.bucket_count(); }

            /// Returns the maximum number of buckets of the %unordered_map.
            size_type
            max_bucket_count() const noexcept
            { return _map.max_bucket_count(); }

            /**
             * @brief  Returns the number of elements in a given bucket.
             * @param  n  A bucket index.
             * @return  The number of elements in the bucket.
             */
            size_type
            bucket_size(size_type n) const
            { return _map.bucket_size(n); }

            /**
             * @brief  Returns the bucket index of a given element.
             * @param  key  A key instance.
             * @return  The key bucket index.
             */
            size_type
            bucket(const key_type& key) const
            { return _map.bucket({key, {}}); }

            /**
             *  @brief  Returns a read/write iterator pointing to the first bucket
             *         element.
             *  @param  n The bucket index.
             *  @return  A read/write local iterator.
             */
            local_iterator
            begin(size_type n)
            { return _map.begin(n); }

            //@{
            /**
             *  @brief  Returns a read-only (constant) iterator pointing to the first
             *         bucket element.
             *  @param  n The bucket index.
             *  @return  A read-only local iterator.
             */
            const_local_iterator
            begin(size_type n) const
            { return _map.begin(n); }

            const_local_iterator
            cbegin(size_type n) const
            { return _map.cbegin(n); }
            //@}

            /**
             *  @brief  Returns a read/write iterator pointing to one past the last
             *         bucket elements.
             *  @param  n The bucket index.
             *  @return  A read/write local iterator.
             */
            local_iterator
            end(size_type n)
            { return _map.end(n); }

            //@{
            /**
             *  @brief  Returns a read-only (constant) iterator pointing to one past
             *         the last bucket elements.
             *  @param  n The bucket index.
             *  @return  A read-only local iterator.
             */
            const_local_iterator
            end(size_type n) const
            { return _map.end(n); }

            const_local_iterator
            cend(size_type n) const
            { return _map.cend(n); }
            //@}

            // hash policy.

            /// Returns the average number of elements per bucket.
            float
            load_factor() const noexcept
            { return _map.load_factor(); }

            /// Returns a positive number that the %unordered_map tries to keep the
            /// load factor less than or equal to.
            float
            max_load_factor() const noexcept
            { return _map.max_load_factor(); }

            /**
             *  @brief  Change the %unordered_map maximum load factor.
             *  @param  ml The new maximum load factor.
             */
            void
            max_load_factor(float ml)
            { _map.load_factor(ml); }

            /**
             *  @brief  May rehash the %unordered_map.
             *  @param  count The new number of buckets.
             *
             *  Rehash will occur only if the new number of buckets respect the
             *  %unordered_map maximum load factor.
             */
            void
            rehash(size_type count)
            { _map.rehash(count); }

            /**
             *  @brief  Prepare the %unordered_map for a specified number of
             *          elements.
             *  @param  count Number of elements required.
             *
             *  Same as rehash(ceil(n / max_load_factor())).
             */
            void
            reserve(size_type count)
            { _map.reserve(count); }

            template<typename Key1, typename T1, typename Hash1, typename Pred1, typename Weight1, typename Policy1,
                    typename Alloc1>
            friend bool
            operator==(const unordered_map<Key1, T1, Hash1, Pred1, Weight1, Policy1, Alloc1>&,
                       const unordered_map<Key1, T1, Hash1, Pred1, Weight1, Policy1, Alloc1>&);

            template<typename Key1, typename T1, typename Hash1, typename Pred1, typename Weight1, typename Policy1,
                    typename Alloc1>
            friend bool
            operator!=(const unordered_map<Key1, T1, Hash1, Pred1, Weight1, Policy1, Alloc1>&,
                       const unordered_map<Key1, T1, Hash1, Pred1, Weight1, Policy1, Alloc1>&);

        private:

            void safeManagerInsert(typename decltype(_map)::iterator it)
            {
                try
                {
                    _manager.insert(&*it);
                }
                catch(...)
                {
                    _map.erase(it);
                    throw;
                }
            }

            bool freeSpace(size_t weight)
            {
#ifndef NDEBUG
                auto startWeight = _manager.weight();
#endif
                if(_manager.weight() < weight)
                {
                    return false;
                }
                size_t freedWeight = 0;
                std::deque<const_replacement_iterator> removed;
                auto next = _manager.next();
                while(freedWeight < weight)
                {
                    if(next == _manager.end())
                    {
                        //Try getting next without hint
                        next = _manager.next();
                        if(next == _manager.end())
                        {
                            for(auto it = removed.rbegin(); it != removed.rend(); ++it)
                            {
                                _manager.reinsert(*it);
                            }
                            return false;
                        }
                    }
                    freedWeight += _manager.calculate_weight(*next);
                    removed.push_back(next);
                    next = _manager.erase(next);
                    next = _manager.next(next);
                }
                for(auto it : removed)
                {
#ifndef NDEBUG
                    auto ret =
#endif
                            _map.erase(*it);
#ifndef NDEBUG
                    assert(ret == 1);
#endif
                }
#ifndef NDEBUG
                assert(startWeight - _manager.weight() >= weight);
#endif
                return true;
            }

            bool freeSpace(size_t weight, const_replacement_iterator except)
            {
#ifndef NDEBUG
                auto startWeight = _manager.weight();
#endif
                if(_manager.weight() < weight)
                {
                    return false;
                }
                size_t freedWeight = 0;
                std::deque<const_replacement_iterator> removed;
                auto next = _manager.next_except(except);
                while(freedWeight < weight)
                {
                    if(next == _manager.end())
                    {
                        //Try getting next without hint
                        next = _manager.next_except(except);
                        if(next == _manager.end())
                        {
                            for(auto it = removed.rbegin(); it != removed.rend(); ++it)
                            {
                                _manager.reinsert(*it);
                            }
                            return false;
                        }
                    }
                    freedWeight += _manager.calculate_weight(*next);
                    removed.push_back(next);
                    next = _manager.erase(next);
                    next = _manager.next_except(next, except);
                }
                for(auto it : removed)
                {
#ifndef NDEBUG
                    auto ret =
#endif
                            _map.erase(*it);
#ifndef NDEBUG
                    assert(ret == 1);
#endif
                }
#ifndef NDEBUG
                assert(startWeight - _manager.weight() >= weight);
#endif
                return true;
            }

            bool provideSpace(size_t weight)
            {
                if(weight > _manager.capacity())
                {
                    return false;
                }
                if(_manager.capacity() - _manager.weight() >= weight)
                {
                    return true;
                }
                return freeSpace(weight - (_manager.capacity() - _manager.weight()));
            }

            bool provideSpace(size_t weight, const_replacement_iterator except)
            {
                if(weight > _manager.capacity())
                {
                    return false;
                }
                if(_manager.capacity() - _manager.weight() >= weight)
                {
                    return true;
                }
                return freeSpace(weight - (_manager.capacity() - _manager.weight()), except);
            }

            void indexInserted(typename decltype(_map)::iterator it)
            {
                if(!provideSpace(_manager.calculate_weight(it->first)))
                {
                    _map.erase(it);
                    throw no_space_error();
                }
                safeManagerInsert(it);
            }

        };

        template<typename Key, typename T, typename Hash, typename Pred, typename Weight, typename Policy,
                typename Alloc>
        bool
        operator==(const unordered_map<Key, T, Hash, Pred, Weight, Policy, Alloc>& lhs,
                   const unordered_map<Key, T, Hash, Pred, Weight, Policy, Alloc>& rhs)
        {
            if (lhs.size() != rhs.size())
                return false;

            for (auto it = lhs.begin(); it != lhs.end(); ++it)
            {
                const auto ity = rhs.quiet_find(it->first);
                if (ity == rhs.end() || !(*ity == *it))
                    return false;
            }
            return true;
        }

        template<typename Key, typename T, typename Hash, typename Pred, typename Weight, typename Policy,
                typename Alloc>
        bool
        operator!=(const unordered_map<Key, T, Hash, Pred, Weight, Policy, Alloc>& lhs,
                   const unordered_map<Key, T, Hash, Pred, Weight, Policy, Alloc>& rhs)
        {
            return !(lhs == rhs);
        }

        template<typename Key, typename T, typename Hash, typename Pred, typename Weight, typename Policy,
                typename Alloc>
        void swap(unordered_map<Key, T, Hash, Pred, Weight, Policy, Alloc>& lhs,
                  unordered_map<Key, T, Hash, Pred, Weight, Policy, Alloc>& rhs)
        {
            lhs.swap(rhs);
        }
    }
}

#endif //EXTENDEDASSOCIATIVECONTAINERS_UNORDERED_MAP_H
