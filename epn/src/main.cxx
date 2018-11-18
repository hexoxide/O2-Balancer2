#include "runFairMQDevice.h"
#include "epn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new EventProccessingNode();
}