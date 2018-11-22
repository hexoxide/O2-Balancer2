#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestExample

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>

#include "o2data.h"

BOOST_AUTO_TEST_SUITE(Test_Example)

BOOST_AUTO_TEST_CASE(TestExample) {
	O2Data test = O2Data();
	BOOST_CHECK(sizeof(test) > 0);
}

BOOST_AUTO_TEST_SUITE_END()