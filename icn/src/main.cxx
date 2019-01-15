#include <string.h>
#include <errno.h>
#include <string>
#include "zookeeper/zookeeper.h"
#include <unistd.h>
#include <stdarg.h>
#include "runFairMQDevice.h"
#include "icn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
    options.add_options()
    	("iterations", bpo::value<uint64_t>()->required(), "Number of iterations for the duration of the experiment");
        // ("rate", bpo::value<uint64_t>()->required(), "frequency of messages per sec");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new InformationControlNode();
}