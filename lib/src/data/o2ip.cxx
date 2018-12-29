#include "o2ip.h"

#include <string>
#include <stdexcept>

using namespace O2::exception;

namespace O2 {
namespace data {

O2Ip::O2Ip() : ip1(0)
	, ip2(0)
	, ip3(0)
	, ip4(0)
{

}

O2Ip::O2Ip(std::string stringRepresentation) throw(O2Exception) : ip1(0)
	, ip2(0)
	, ip3(0)
	, ip4(0)
{
	try {
		int iIp1, iIp2, iIp3, iIp4;
		std::string sIp1, sIp2, sIp3, sIp4;
		size_t dot1, dot2, dot3;

		// start character until '.' is first ip segment
		dot1 = stringRepresentation.find('.');
		sIp1 = stringRepresentation.substr(0, dot1);

		// find the next '.' based on the length of the first segment + 2 for a off-by-one and to account for the '.' character
		dot2 = stringRepresentation.find('.', sIp1.length() + 2);
		// find the second segment based on the length of the first + 1 for the '.' the length is the difference between the dots -1 for one included '.'
		sIp2 = stringRepresentation.substr(sIp1.length() + 1, dot2 - dot1 - 1);

		// find the next '.' based on the length of the first and second segments + 2 for a off-by-one and to account for the '.' character
		dot3 = stringRepresentation.find('.', sIp1.length() + sIp2.length() + 2);
		// find the third segment based on the length of the first and second + 2 for the both '.''s 
		// the length is the difference between the dots -1 for one included '.'
		sIp3 = stringRepresentation.substr(sIp1.length() + sIp2.length() + 2, dot3 - dot2 - 1);

		// find the fourth segment based on the lengths of the first, second and third + 3 for the three included '.''s
		sIp4 = stringRepresentation.substr(sIp1.length() + sIp2.length() + sIp3.length() + 3);

		iIp1 = stoi(sIp1);
		iIp2 = stoi(sIp2);
		iIp3 = stoi(sIp3);
		iIp4 = stoi(sIp4);

		// Constrain inputs and if invalid throw an exception
		if(iIp1 < 0 || iIp1 > 255) {
			throw std::overflow_error("ip segment one outside of range");
		}

		if(iIp2 < 0 || iIp2 > 255) {
			throw std::overflow_error("ip segment two outside of range");
		}

		if(iIp3 < 0 || iIp3 > 255) {
			throw std::overflow_error("ip segment three outside of range");
		}

		if(iIp4 < 0 || iIp4 > 255) {
			throw std::overflow_error("ip segment four outside of range");
		}

		// set the actual values
		ip1 = iIp1;
		ip2 = iIp2;
		ip3 = iIp3;
		ip4 = iIp4;
	}
	catch(std::exception& e) {
		throw O2Exception(e.what(), __FILE__, __LINE__);
	}
}

O2Ip::operator std::string() {
	return std::to_string(ip1) + "." +  std::to_string(ip2) + "." + std::to_string(ip3) + "." + std::to_string(ip4);
}

} // data
} // O2