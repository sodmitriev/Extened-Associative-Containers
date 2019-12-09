# Copyright 2019 Sviatoslav Dmitriev
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

add_executable(unordered_map_constructor_test ../unordered_map.h ../base.h unordered_map_constructor_test.cpp)
target_compile_options(unordered_map_constructor_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_constructor_test unordered_map_constructor_test)
enable_testing()

add_executable(unordered_map_copy_test ../unordered_map.h ../base.h unordered_map_copy_test.cpp)
target_compile_options(unordered_map_copy_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_copy_test unordered_map_copy_test)
enable_testing()

add_executable(unordered_map_erase_test ../unordered_map.h ../base.h unordered_map_erase_test.cpp)
target_compile_options(unordered_map_erase_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_erase_test unordered_map_erase_test)
enable_testing()

add_executable(unordered_map_hash_policy_interface_test ../unordered_map.h ../base.h unordered_map_hash_policy_interface_test.cpp)
target_compile_options(unordered_map_hash_policy_interface_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_hash_policy_interface_test unordered_map_hash_policy_interface_test)
enable_testing()

add_executable(unordered_map_insertion_test ../unordered_map.h ../base.h unordered_map_insertion_test.cpp)
target_compile_options(unordered_map_insertion_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_insertion_test unordered_map_insertion_test)
enable_testing()

add_executable(unordered_map_iterators_test ../unordered_map.h ../base.h unordered_map_iterators_test.cpp)
target_compile_options(unordered_map_iterators_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_iterators_test unordered_map_iterators_test)
enable_testing()

add_executable(unordered_map_lookup_test ../unordered_map.h ../base.h unordered_map_lookup_test.cpp)
target_compile_options(unordered_map_lookup_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_lookup_test unordered_map_lookup_test)
enable_testing()

add_executable(unordered_map_replacement_interface_test ../unordered_map.h ../base.h unordered_map_replacement_interface_test.cpp)
target_compile_options(unordered_map_replacement_interface_test PRIVATE "-Wno-sign-compare")
add_test(unordered_map_replacement_interface_test unordered_map_replacement_interface_test)

add_executable(performance_test performance_test.cpp ../unordered_map.h ../base.h)
