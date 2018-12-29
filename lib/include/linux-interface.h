#ifndef LINUX_INTERFACE_H
#define LINUX_INTERFACE_H

// c headers
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// cxx headers
#include <string>

#include "data/o2ip.h"
#include "o2exception.h"

namespace O2 {
namespace network {

O2::data::O2Ip getInterfaceAddress(std::string interfaceName);

}
}

#endif /* LINUX_INTERFACE_H */