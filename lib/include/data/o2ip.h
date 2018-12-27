#ifndef O2IP_H
#define O2IP_H

#include <stdint.h>
#include <string>

struct O2Ip {
	uint8_t		ip1;
	uint8_t		ip2;
	uint8_t		ip3;
	uint8_t		ip4;
	O2Ip();
	operator std::string();
};

#endif // O2IP