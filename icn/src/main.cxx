#include "runFairMQDevice.h"
#include "icn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
    options.add_options();
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new InformationControlNode();
}
