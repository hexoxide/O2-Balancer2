[![travis](https://api.travis-ci.com/hexoxide/O2-Balancer2.svg?branch=zookeepertje)](https://travis-ci.com/hexoxide/O2-Balancer2) [![codecov](https://codecov.io/gh/hexoxide/O2-Balancer2/branch/zookeepertje/graph/badge.svg)](https://codecov.io/gh/hexoxide/O2-Balancer2)

# Load balancer V3 for ALICE - Zookeeper whitelist 
In collaboration with the Amsterdam University of Applied sciences and CERN a load balancer is being developed for the ALICE O2 project.

## Description
This zookeeper branch uses zookeeper to maintain which epn is online, the flp retrieves this list from zookeeper and uses the round robin to distribute the the load among these epns. The epn is very simple it creates a zookeeper node in the zooker /EPN/ directory and then whenever it goes offline the flp will be noticed (because it has a watcher placed on the /EPN/ node).<br/>
It is a whitelist implementation beacause it maintains the list of online nodes, when the watcher notifies that a node has gone offline, the flp will remove the node from its list of available nodes. The round robin algorithm will then skip it until it comes back online.<br/>
The ICN is very minimalistic: it only sends heartbeats at a certain channel (to which the flps connect). This is to mock the heartbeats that will be given to the flp by the sensors in the ALICE detector. <br/>
The FLP will be sending messages as soon as there is atleast 1 epn online and when there is 1 icn sending heartbeats.

* ICN
* FLP
* EPN

## Dependencies
To ensure the O2Balancer2 remains functional even years after it has been developed all dependencies need to be made easilty available. To ensure this a special repository is setup which uses git submodules to checkout specific versions of dependencies.

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
Example experiment run with 2 pi clusters (8 pi's):
First "git checkout zookeepertje" on all nodes, and go to O2balancer2/build/ and run "make"
```bash
node 1:
sudo /usr/share/zookeeper/bin/zkCli.sh start
./icn/icn --severity trace --verbosity high --id 1 --iterations 6000 --rate 200 --channel-config name=broadcast,type=pub,method=bind,rateLogging=0,address=tcp://*:5005

node 2 and 5:
./flp/flp --severity trace --verbosity high --id 1 --channel-config name=broadcast,type=sub,method=connect,rateLogging=1,address=tcp://192.168.1.1:5005

node 3,4,6,7 and 8 (dit address is de nodes zijn eigen address)
./epn/epn --severity trace --verbosity high --id 1 --channel-config name=1,type=pull,method=bind,address=tcp://192.168.1.x:5555,rateLogging=1

```

## Debugging
Start individual applications of the O2 balancer using gdb and be sure to pipe the output to a file or null. Be sure that the binaries are compiled with gcc `-g -O0` arguments by setting the cmake `DCMAKE_BUILD_TYPE` to `Debug`.

```bash
gdb -tui PATH_TO_BINARY
run ARGUMENTS > output.log
```
