#ifndef O2DATA_H
#define O2DATA

#include <stdint.h>

class O2Data {
public:
	O2Data();
	uint64_t	heartbeat;
	uint64_t	tarChannel;
	bool 		configure;
protected:
private:
};

#endif // O2DATA
