#define BOOST_TEST_MODULE regez_tests
#include <boost/test/included/unit_test.hpp>
#include "regez.hpp"

BOOST_AUTO_TEST_CASE(state_id_check)
{
    regez::state<int> s1;
    regez::state<int> s2;
    regez::state<int> s3;
    regez::state<int> s4;
    regez::state<int> s5;
    BOOST_TEST(s1.get_id() == 0);
    BOOST_TEST(s2.get_id() == 1);
    BOOST_TEST(s3.get_id() == 2);
    BOOST_TEST(s4.get_id() == 3);
    BOOST_TEST(s5.get_id() == 4);
}
