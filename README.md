![https://travis-ci.com/hexoxide/O2-Balancer2](https://api.travis-ci.com/hexoxide/O2-Balancer2.svg?branch=master)

# Load balancer V2 for ALICE
In collaboration with the Amsterdam University of Applied sciences and CERN a load balancer is being developed for the ALICE O2 project.

## Operation
The balancer is build out of three parts each with its own purposes and characteristics. 

* ICN
* FLP
* EPN

## Installation

```bash
if [ ! -d "build" ]; then mkdir build; fi
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j 2
```

## Running

```bash
./icn/icn --severity trace --verbosity veryhigh --id 1 --rate 50 --channel-config name=broadcast,type=pub,method=bind,rateLogging=0,address=tcp://*:5005
./flp/flp --severity trace --verbosity veryhigh --id 1 --rate 50 --bytes-per-message 2097152 --channel-config name=1,type=push,method=bind,address=tcp://*:5555,rateLogging=1 name=broadcast,type=sub,method=connect,rateLogging=1,address=tcp://localhost:5005
./epn/epn --severity trace --verbosity veryhigh --id 1 --channel-config name=1,type=pull,method=connect,address=tcp://localhost:5555,rateLogging=1
```

## Debugging
Start individual applications of the O2 balancer using gdb and be sure to pipe the output to a file or null.
```bash
gdb -tui FILE
run > output.log
```
