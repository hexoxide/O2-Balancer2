#ifndef O2CHANNEL_H
#define O2CHANNEL_H

#include <stdint.h>

struct O2Channel {
	uint64_t	index;
	uint8_t		ip1;
	uint8_t		ip2;
	uint8_t		ip3;
	uint8_t		ip4;
	uint16_t	port;
	constexpr O2Channel()
		: index(0)
		, ip1(0)
		, ip2(0)
		, ip3(0)
		, ip4(0)
		, port(1) {}
};

#endif // O2CHANNEL
