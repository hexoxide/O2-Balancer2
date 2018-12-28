#include "runFairMQDevice.h"
#include "epn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
	options.add_options()
		(EventProccessingNode::pNumberOfFLP.c_str(), bpo::value<uint64_t>()->required(), "Number of FLP's the EPN will expect to be in the network.")
		(EventProccessingNode::pPrimaryNetworkInterface.c_str(), bpo::value<std::string>()->required(), "The primary network interface to use while performing load balancing experiments");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new EventProccessingNode();
}