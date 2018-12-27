#ifndef O2DATA_H
#define O2DATA_H

#include <stdint.h>

struct O2Data {
	uint64_t	heartbeat;
	uint64_t	tarChannel;
	bool 		configure;
	constexpr O2Data()
		: heartbeat(0)
		, tarChannel(0)
		, configure(false) {}
};

#endif // O2DATA
