#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEpn

#include <boost/test/unit_test.hpp>
#include <string>

#include "epn.h"

#include "data/o2data.h"
#include "data/o2channel.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Test_Epn)

BOOST_AUTO_TEST_CASE(TestEpn) {
	O2Data data = O2Data();
	BOOST_CHECK(sizeof(data) > 0);
}

BOOST_AUTO_TEST_SUITE_END()
