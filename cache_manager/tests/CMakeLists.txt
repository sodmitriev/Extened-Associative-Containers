add_executable(cache_manager_test cache_manager_test.cpp ../cache_manager.h ../replacement_iterator.h ../policy.h ../base.h)
target_compile_options(cache_manager_test PRIVATE "-Wno-sign-compare")
add_test(cache_manager_test cache_manager_test)

add_executable(replacement_iterator_test replacement_iterator_test.cpp ../cache_manager.h ../replacement_iterator.h ../policy.h ../base.h)
add_test(replacement_iterator_test replacement_iterator_test)

add_executable(policy_test policy_test.cpp ../cache_manager.h ../replacement_iterator.h ../policy.h ../base.h)
add_test(policy_test policy_test)