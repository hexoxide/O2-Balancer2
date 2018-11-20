#include "runFairMQDevice.h"
#include "epn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
	options.add_options()
		("num-flp", bpo::value<uint64_t>()->required(), "Number of bytes to transmit per message");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new EventProccessingNode();
}