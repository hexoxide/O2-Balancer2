#include "runFairMQDevice.h"
#include "flp.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
    options.add_options()
    	("bytes-per-message", bpo::value<uint64_t>()->default_value(20480), "Number of bytes to transmit per message")
        ("max-iterations", bpo::value<uint64_t>()->default_value(0), "Maximum number of iterations of Run/ConditionalRun/OnData (0 - infinite)");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new FirstLineProccessing();
}