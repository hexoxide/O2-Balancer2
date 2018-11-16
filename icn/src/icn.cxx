#include "icn.h"

using namespace std;

InformationControlNode::InformationControlNode()
    : fText()
    , fMaxIterations(0)
    , fNumIterations(0)
{
}

void InformationControlNode::InitTask()
{
    // Get the fText and fMaxIterations values from the command line options (via fConfig)
    fText = fConfig->GetValue<string>("text");
    fMaxIterations = fConfig->GetValue<uint64_t>("max-iterations");
}

bool InformationControlNode::ConditionalRun()
{
    // create a copy of the data with new(), that will be deleted after the transfer is complete
    string* text = new string(fText);

    // create message object with a pointer to the data buffer,
    // its size,
    // custom deletion function (called when transfer is done),
    // and pointer to the object managing the data buffer
    FairMQMessagePtr msg(NewMessage(const_cast<char*>(text->c_str()),
                                    text->length(),
                                    [](void* /*data*/, void* object) { delete static_cast<string*>(object); },
                                    text));

    LOG(info) << "Sending \"" << fText << "\"";

    // in case of error or transfer interruption, return false to go to IDLE state
    // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
    if (Send(msg, "data") < 0)
    {
        return false;
    }
    else if (fMaxIterations > 0 && ++fNumIterations >= fMaxIterations)
    {
        LOG(info) << "Configured maximum number of iterations reached. Leaving RUNNING state.";
        return false;
    }

    return true;
}

InformationControlNode::~InformationControlNode()
{
}