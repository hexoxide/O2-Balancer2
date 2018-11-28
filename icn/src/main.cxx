#include "runFairMQDevice.h"
#include "icn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
    options.add_options()
    	("iterations", bpo::value<uint64_t>()->required(), "Number of iterations for the duration of the experiment");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new InformationControlNode();
}
