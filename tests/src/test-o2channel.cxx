#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestO2Channel

#include <boost/test/unit_test.hpp>
#include <string>

#include "data/o2channel.h"

BOOST_AUTO_TEST_SUITE(Test_O2Channel)

BOOST_AUTO_TEST_CASE(TestO2ChannelMininum) {
	auto test = O2Channel();
	test.index = 0;
	test.ip1 = 0;
	test.ip2 = 0;
	test.ip3 = 0;
	test.ip4 = 0;
	test.port = 1;
	BOOST_CHECK(std::string(test) == "0:0.0.0.0:1");
}

BOOST_AUTO_TEST_CASE(TestO2ChannelMaximum) {
	auto test = O2Channel();
	test.index = UINT64_MAX;
	test.ip1 = UINT8_MAX;
	test.ip2 = UINT8_MAX;
	test.ip3 = UINT8_MAX;
	test.ip4 = UINT8_MAX;
	test.port = UINT16_MAX;
	BOOST_CHECK(std::string(test) == "18446744073709551615:255.255.255.255:65535");
}

BOOST_AUTO_TEST_SUITE_END()