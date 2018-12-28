#include "linux-interface.h"

namespace O2 {
namespace network {

	std::string getInterfaceAddress(std::string interfaceName) {
		std::string ip = "";
		struct ifaddrs *ifaddr, *ifa;
	    int s;
	    char host[NI_MAXHOST];

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
	                throw std::runtime_error("Interface does not have ipv4 address attribute (AF_INET)");
	            }
	            ip = std::string(host);
	        }
	    }
	    freeifaddrs(ifaddr);
	   	return ip;
	}

}
}