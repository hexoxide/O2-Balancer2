#include "o2channel.h"

O2Channel::O2Channel() : O2Ip()
	, index(0)
	, port(1)
{

}

O2Channel::operator std::string() {
	return std::to_string(index) + ":" + std::to_string(ip1) + "." +  std::to_string(ip2) + "." + std::to_string(ip3) + "." + std::to_string(ip4) + ":" + std::to_string(port);
}

// O2Channel::std::string() {
// 	return to_string(ip1) + "." +  to_string(ip2) + "." + to_string(ip3) + "." + to_string(ip4);
// }