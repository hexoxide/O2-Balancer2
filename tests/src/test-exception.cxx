#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestException

#include <boost/test/unit_test.hpp>
#include <string>

#include "o2exception.h"

using namespace O2::exception;

BOOST_AUTO_TEST_SUITE(Test_Exception)

BOOST_AUTO_TEST_CASE(TestExceptionOutput) {
	std::string prefix = "start"; 
	std::string postfix = ":16"; // ensure the number matches the line number in the __LINE__ instruction!
	O2Exception test = O2Exception(prefix, __FILE__, __LINE__); // Create O2Exception and correctly pass __FILE__ and __LINE__

	std::string exceptionWhat = std::string(test.what()); // convert the exception char* into a std::string

	BOOST_CHECK(!exceptionWhat.compare(0, prefix.size(), prefix)); // Validate the what() output starts with the contents of prefix
	BOOST_CHECK(std::equal(postfix.rbegin(), postfix.rend(), exceptionWhat.rbegin())); // validate the what() output ends with the content of  __LINE__
}

BOOST_AUTO_TEST_SUITE_END()