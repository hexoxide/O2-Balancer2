#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestO2Ip

#include <boost/test/unit_test.hpp>
#include <string>

#include "data/o2ip.h"

BOOST_AUTO_TEST_SUITE(Test_O2Ip)

BOOST_AUTO_TEST_CASE(TestO2IpMinimum) {
	auto test = O2Ip();
	test.ip1 = 0;
	test.ip2 = 0;
	test.ip3 = 0;
	test.ip4 = 0;
	BOOST_CHECK(std::string(test) == "0.0.0.0");
}

BOOST_AUTO_TEST_CASE(TestO2IpHalf) {
	auto test = O2Ip();
	test.ip1 = 128;
	test.ip2 = 128;
	test.ip3 = 128;
	test.ip4 = 128;
	BOOST_CHECK(std::string(test) == "128.128.128.128");
}

BOOST_AUTO_TEST_CASE(TestO2Localhost) {
	auto test = O2Ip();
	test.ip1 = 127;
	test.ip2 = 0;
	test.ip3 = 0;
	test.ip4 = 1;
	BOOST_CHECK(std::string(test) == "127.0.0.1");
}

BOOST_AUTO_TEST_CASE(TestO2IpMaximum) {
	auto test = O2Ip();
	test.ip1 = UINT8_MAX;
	test.ip2 = UINT8_MAX;
	test.ip3 = UINT8_MAX;
	test.ip4 = UINT8_MAX;
	BOOST_CHECK(std::string(test) == "255.255.255.255");
}

BOOST_AUTO_TEST_SUITE_END()