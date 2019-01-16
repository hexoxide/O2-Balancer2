#include "runFairMQDevice.h"
#include "flp.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
    options.add_options()
    	("bytes-per-message", bpo::value<uint64_t>()->default_value(232000), "Number of bytes to transmit per message");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new FirstLineProccessing();
}