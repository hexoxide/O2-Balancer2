#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestO2Networking

#include <boost/test/unit_test.hpp>
#include <string>

#include "linux-interface.h"

using namespace O2::network;

BOOST_AUTO_TEST_SUITE(Test_O2Networking)

BOOST_AUTO_TEST_CASE(TestO2LocalAddress) {
	std::string test = getInterfaceAddress("lo");
	BOOST_CHECK(test == "127.0.0.1");
}

BOOST_AUTO_TEST_SUITE_END()