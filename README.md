[![travis](https://api.travis-ci.com/hexoxide/O2-Balancer2.svg?branch=master)](https://travis-ci.com/hexoxide/O2-Balancer2) [![codecov](https://codecov.io/gh/hexoxide/O2-Balancer2/branch/master/graph/badge.svg)](https://codecov.io/gh/hexoxide/O2-Balancer2)

# Load balancer V2 for ALICE
In collaboration with the Amsterdam University of Applied sciences and CERN a load balancer is being developed for the ALICE O2 project.

## Operation
The balancer is build out of three parts each with its own purposes and characteristics. 

* ICN
* FLP
* EPN

## Dependencies

```bash
git clone https://github.com/hexoxide/raspberry-dependency.git
cd raspberry-dependency
git checkout O2v2
su root
apt update
apt upgrade
apt install sudo gcc git wget htop make icu-devtools python python-dev ant libcppunit-dev 
apt install doxygen automake autoconf libtool zookeeper libzookeeper-mt-dev
./install-dependencies.sh
```

## Installation

```bash
if [ ! -d "build" ]; then mkdir build; fi
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j 2
```

## Running

```bash
./icn/icn --severity trace --verbosity high --id 1 --rate 50 --channel-config name=broadcast,type=pub,method=bind,rateLogging=1,address=tcp://*:5005 name=feedback,type=sub,method=bind,rateLogging=1,address=tcp://*:5000
./flp/flp --severity trace --verbosity high --id 1 --rate 50 --bytes-per-message 2097152 --channel-config name=1,type=push,method=bind,address=tcp://*:5555,rateLogging=1 name=broadcast,type=sub,method=connect,rateLogging=1,address=tcp://localhost:5005
./epn/epn --severity trace --verbosity high --id 1 --num-flp 10 --channel-config name=1,type=pull,method=connect,address=tcp://localhost:5555,rateLogging=1 name=feedback,type=pub,method=connect,address=tcp://localhost:5000
```

**Old method - Commit 978c07854b038804073db5d7a927bdabf9292dd2**

```bash
./icn/icn --severity trace --verbosity veryhigh --id 1 --rate 50 --channel-config name=broadcast,type=pub,method=bind,rateLogging=1,address=tcp://*:5005 name=feedback,type=pull,method=bind,rateLogging=1,address=tcp://*:5000
./flp/flp --severity trace --verbosity veryhigh --id 1 --rate 50 --bytes-per-message 2097152 --channel-config name=1,type=push,method=bind,address=tcp://*:5555,rateLogging=1 name=broadcast,type=sub,method=connect,rateLogging=1,address=tcp://localhost:5005
./epn/epn --severity trace --verbosity veryhigh --id 1 --channel-config name=1,type=pull,method=connect,address=tcp://localhost:5555,rateLogging=1 name=feedback,type=push,method=connect,address=tcp://localhost:5000
```

## Debugging
Start individual applications of the O2 balancer using gdb and be sure to pipe the output to a file or null. Be sure that the binaries are compiled with gcc `-g -O0` arguments by setting the cmake `DCMAKE_BUILD_TYPE` to `Debug`.

```bash
gdb -tui PATH_TO_BINARY
run ARGUMENTS > output.log
```

## Working on serialization

```
OFI::libfabric protobuf::libprotobuf $<TARGET_OBJECTS:OfiTransport>

CMAKE:
  find_package2(PRIVATE OFI VERSION 1.6.0 REQUIRED COMPONENTS fi_sockets fi_verbs)
  find_package2(PRIVATE Protobuf VERSION 3.4.0 REQUIRED)
```