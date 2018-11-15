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

## Debugging
Start individual applications of the O2 balancer using gdb and be sure to pipe the output to a file or null.
```bash
gdb -tui FILE
run > output.log
```