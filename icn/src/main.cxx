#include "runFairMQDevice.h"
#include "icn.h"

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
    options.add_options()
        ("text", bpo::value<std::string>()->default_value("Hello"), "Text to send out")
        ("max-iterations", bpo::value<uint64_t>()->default_value(0), "Maximum number of iterations of Run/ConditionalRun/OnData (0 - infinite)");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
    return new InformationControlNode();
}

// int main(int argc, const char* argv[]) {

// }