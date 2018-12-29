[![travis](https://api.travis-ci.com/hexoxide/O2-Balancer2.svg?branch=master)](https://travis-ci.com/hexoxide/O2-Balancer2) [![codecov](https://codecov.io/gh/hexoxide/O2-Balancer2/branch/master/graph/badge.svg)](https://codecov.io/gh/hexoxide/O2-Balancer2)

# Load balancer V2 for ALICE
In collaboration with the Amsterdam University of Applied sciences and CERN a load balancer is being developed for the ALICE O2 project.

## Operation
The balancer is build out of three parts each with its own purposes and characteristics. Running experiments should have atleast one of each parts running. The InformationControlNode (ICN) orchestrates the experiments and determines parameters such as durations & rate.

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
apt install doxygen graphviz automake autoconf libtool zookeeper libzookeeper-mt-dev
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
./icn/icn --control static --severity trace --verbosity high --id 1 --iterations 500 --rate 50 --channel-config name=broadcast,type=pub,method=bind,rateLogging=1,address=tcp://*:5005 name=feedback,type=pull,method=bind,rateLogging=1,address=tcp://*:5000
./flp/flp --severity trace --verbosity high --id 1 --bytes-per-message 2097152 --channel-config name=broadcast,type=sub,method=connect,rateLogging=1,address=tcp://localhost:5005
./epn/epn --control static --severity trace --verbosity high --id 1 --num-flp 10 --channel-config name=1,type=pull,method=bind,address=tcp://localhost:5555,rateLogging=1 name=feedback,type=push,method=connect,address=tcp://localhost:5000
```

### Operational parameters


## Debugging
Start individual applications of the O2 balancer using gdb and be sure to pipe the output to a file or null. Be sure that the binaries are compiled with gcc `-g -O0` arguments by setting the cmake `DCMAKE_BUILD_TYPE` to `Debug`.

```bash
gdb -tui PATH_TO_BINARY
run ARGUMENTS > output.log
```