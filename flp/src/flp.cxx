#include "flp.h"

using namespace std;

FirstLineProccessor::FirstLineProccessor()
    : fMaxIterations(0)
    , fNumIterations(0)
{
    // register a handler for data arriving on "data" channel
    OnData("data", &FirstLineProccessor::HandleData);
}

void FirstLineProccessor::InitTask()
{
    // Get the fMaxIterations value from the command line options (via fConfig)
    fMaxIterations = fConfig->GetValue<uint64_t>("max-iterations");
}

// handler is called whenever a message arrives on "data", with a reference to the message and a sub-channel index (here 0)
bool FirstLineProccessor::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
    // LOG(info) << "Received: \"" << string(static_cast<char*>(msg->GetData()), msg->GetSize()) << "\"";

    if (fMaxIterations > 0 && ++fNumIterations >= fMaxIterations)
    {
        LOG(info) << "Configured maximum number of iterations reached. Leaving RUNNING state.";
        return false;
    }

    // return true if want to be called again (otherwise return false go to IDLE state)
    return true;
}

FirstLineProccessor::~FirstLineProccessor()
{
}