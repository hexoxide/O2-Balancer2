#ifndef O2IP_H
#define O2IP_H

// C headers
#include <stdint.h>

// CXX headers
#include <string>

// library headers
#include "o2exception.h"

namespace O2 {
namespace data {

struct O2Ip {
	uint8_t		ip1;
	uint8_t		ip2;
	uint8_t		ip3;
	uint8_t		ip4;
	O2Ip();
	O2Ip(std::string stringRepresentation) throw(O2::exception::O2Exception);
	operator std::string();
};

} // data
} // O2

#endif // O2IP