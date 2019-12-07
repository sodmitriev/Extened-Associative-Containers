# C++ extended associative containers

This project implements a set of custom associative containers, that are based on standard c++ containers but provide 
an extended functionality.

All containers are based on a corresponding standard container implementation, and are aimed at providing extended 
functionality without a significant overhead.

Extended containers use a similar to standard container interface.

This project includes custom extensions above the following associative containers:

* map
* set
* unordered_map
* unordered_set
* multimap
* multiset
* unordered_map
* unordered_set

For each container there are two extensions, that can be used simultaneously:

* limited - container acts like a cache based on a provided replacement policy
* multikey - container can have multiple independent keys

## Limited associative containers

Provides a set of containers that can be used as a low overhead cache. Complexity of a limited container's methods
 always equals to the maximum between a complexity of a base standard container's methods and complexity of replacement 
 algorithm. All replacement algorithms that are provided with this project have a complexity of O(1).
 
### Insertion

Limited associative containers provide same insertion methods as standard containers. 

 When an element is inserted into 
 a container that does not have enough free space, one or more old elements will be erased from the container,
 corresponding to a replacement policy.
 
 If container can't provide enough space to store an element, exception will be thrown.
 
### Lookup

Limited associative containers provide same lookup methods as standard containers. When a lookup operation is performed,
 the accessed method will change it's position in replacement queue, which makes lookup methods non constant and non
 thread safe. 
 
 To provide constant and thread safe interface extended containers provide a set of lookup methods suffixed
 with `quiet` (e.g. `quiet_find(const key_type& key)`). Quiet methods will not modify element position in the 
 replacement queue.
 
### Weighting elements

Limited containers provide an interface to specify a relative size of a stored value. By default, all elements have a 
 size of 1, and thus in a container with capacity on N can be stored exactly N elements.
 
 A functor object is used to calculate element weight. The weight functor can be redefined in order to adjust caching
 efficiency.
 
 For example, if container contains has a capacity of 10 and contains 3 elements with capacities 1, 2 and 3, then 
 another element with weight of 4 or less can be inserted inserted into the container without affecting existing 
 elements. However, if an element with weight of more that 4 is inserted into the container, one of old elements will
 be erased.
 
 Notice that when using a custom weight functor size of an element must stay constant throughout the time that element
 is stored within a container. Changing a stored element weight will lead to and unidentified behaviour. To safely
 modify an element's weight it's advised to `extract` element from the container, modify that element and `insert` it 
 back. This, however, will reset it's position in replacement queue, which may be inappropriate for some replacement 
 algorithms.
 
### Direct replacement control

Replacement interface can be used to explicitly modify an element's position in a replacement queue. Container's
methods `replacement_begin()`, `replacement_end()` and `replacement_find()` can be used to receive an iterator that 
addresses stored elements in their order in the replacement queue. Elements that are closer to `replacement_begin()` are
more likely to be replaced by new elements, while elements that are closer to `replacement_end()` are unlikely to be 
replaced.

A set of operations that can be found in cache_manager::iterator_operations class can be used to directly modify 
element's position in the replacement queue.

### Replacement policy

Custom replacement policy can be specified for limited containers. Following replacement policies are provided in the
cache_manager::policy namespace:

* fifo
* lifo
* lru
* mru
* swapping (moves element one position further form replacement_begin each time the element is accessed)

Along with that several policy adaptors are provided:

* simple_locked_policy (allows locking elements form being erased)
* priority_policy (prefers elements with lesser priority)

To define a custom policy, you can refer to example_replacement_algorithm. A custom policy can provide any combination
of required methods. If a method is not provided, it will be defaulted in policy_extractor class. For description of
default behaviours refer to policy_extractor class.

## Multikey associative containers

This section is in specification

## Container development status

Development status of extended associative container can be found [here](STATUS.md).