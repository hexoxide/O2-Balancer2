#ifndef LINUX_INTERFACE_H
#define LINUX_INTERFACE_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <string>

std::string getInterfaceAddress(std::string interfaceName);

#endif /* LINUX_INTERFACE_H */