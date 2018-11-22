#ifndef O2CHANNEL_H
#define O2CHANNEL

#include <stdint.h>

class O2Channel {
public:
	O2Channel();
	uint64_t	index;
	uint8_t		ip1;
	uint8_t		ip2;
	uint8_t		ip3;
	uint8_t		ip4;
	uint16_t	port;
protected:
private:
};

#endif // O2CHANNEL
