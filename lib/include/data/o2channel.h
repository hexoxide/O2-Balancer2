#ifndef O2CHANNEL_H
#define O2CHANNEL_H

#include <stdint.h>
#include <string>

#include "o2ip.h"

struct O2Channel : O2Ip {
	uint64_t	index;
	uint16_t	port;
	O2Channel();
	operator std::string();
};

#endif // O2CHANNEL
