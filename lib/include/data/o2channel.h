#ifndef O2CHANNEL_H
#define O2CHANNEL_H

#include <stdint.h>
#include <string>

#include "o2ip.h"

namespace O2 {
namespace data {

struct O2Channel : O2Ip {
	uint64_t	index;
	uint16_t	port;
	O2Channel();
	operator std::string();
};

} // data
} // O2

#endif // O2CHANNEL
