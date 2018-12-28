#include "o2ip.h"

namespace O2 {
namespace data {

O2Ip::O2Ip() : ip1(0)
	, ip2(0)
	, ip3(0)
	, ip4(0)
{

}

O2Ip::operator std::string() {
	return std::to_string(ip1) + "." +  std::to_string(ip2) + "." + std::to_string(ip3) + "." + std::to_string(ip4);
}

} // data
} // O2