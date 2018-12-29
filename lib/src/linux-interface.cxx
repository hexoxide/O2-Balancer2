#include "linux-interface.h"

using namespace O2::exception;

namespace O2 {
namespace network {

	std::string getInterfaceAddress(std::string interfaceName) {
		std::string ip = std::string();
		struct ifaddrs *ifaddr, *ifa;
	    int s = 0;
	    char host[NI_MAXHOST] = {};

	    if (getifaddrs(&ifaddr) == -1) 
	    {
	        throw std::runtime_error("Could not open network interface");
	    }

	    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
	    {
	        if (ifa->ifa_addr == NULL)
	            continue;  

	        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

	        if((strcmp(ifa->ifa_name, interfaceName.c_str())==0)&&(ifa->ifa_addr->sa_family==AF_INET))
	        {
	            if (s != 0)
	            {
	                throw O2Exception("Could not read AF_INET for network interface", __FILE__, __LINE__);
	            }
	            ip = std::string(host);
	        }
	    }

	    if(ip.empty())
	    	throw O2Exception("Could not retrieve AF_INET for interface: " + interfaceName, __FILE__, __LINE__);

	    freeifaddrs(ifaddr);
	   	return ip;
	}

}
}