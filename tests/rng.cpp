#include "../include/rng.hpp"

#define BOOST_TEST_MODULE pagmo_rng_test
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>

using namespace pagmo;

BOOST_AUTO_TEST_CASE(set_seed_and_next)
{
    // We check that the first N pseudo random numbers are identical if generated
    // right after the same seed is set and different otherwise.

    // We choose two seeds 
    detail::random_engine_type::result_type seed{0u};
    detail::random_engine_type::result_type seed2{1u};

    // Length of the pseudo-random sequence tested 
    unsigned int N = 10000u;

    // We generate three pseudo random sequences, two with the same seed
    random_device::set_seed(seed);
    std::vector<detail::random_engine_type::result_type> prs1;
    std::generate_n(std::back_inserter(prs1),N,random_device::next);	

    random_device::set_seed(seed);
    std::vector<detail::random_engine_type::result_type> prs2;
    std::generate_n(std::back_inserter(prs2),N,random_device::next);

    random_device::set_seed(seed2);
    std::vector<detail::random_engine_type::result_type> prs3;
    std::generate_n(std::back_inserter(prs3),N,random_device::next);

    // We check that prs1 and prs2 are equal, since the seed was the same
    BOOST_CHECK(std::equal(prs1.begin(),prs1.end(),prs2.begin()));
    // We check that prs1 are prs3 are different since the seed was different
    BOOST_CHECK(!std::equal(prs1.begin(),prs1.end(),prs3.begin()));
}

// This test just runs calls to random_device::next() in two separate threads. If this executable
// is compiled with -fsanitize=thread in clang/gcc, it should check that the locking logic
// in random_device is correct.
BOOST_AUTO_TEST_CASE(data_races_test)
{
    unsigned int N = 10000u;
    std::vector<detail::random_engine_type::result_type> prs4, prs5;
    std::thread t1([&]() {std::generate_n(std::back_inserter(prs4),N,random_device::next);});
    std::thread t2([&]() {std::generate_n(std::back_inserter(prs5),N,random_device::next);});
    t1.join();
    t2.join();
}