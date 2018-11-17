#include "icn.h"

using namespace std;

InformationControlNode::InformationControlNode()
    : fTextSize(0)
    , fMaxIterations(0)
    , fNumIterations(0)
{
}

void InformationControlNode::InitTask()
{
    // Get the fText and fMaxIterations values from the command line options (via fConfig)
    fTextSize = fConfig->GetValue<uint64_t>("bytes-per-message");
    fMaxIterations = fConfig->GetValue<uint64_t>("max-iterations");

    text = new char[fTextSize];

    for(uint64_t i = 0; i < fTextSize; i++) {
        text[i] = 'a';
    }
}

bool InformationControlNode::ConditionalRun()
{
    // create message object with a pointer to the data buffer,
    // its size,
    // custom deletion function (called when transfer is done),
    // and pointer to the object managing the data buffer
    FairMQMessagePtr msg(NewMessage(text,
                                    fTextSize,
                                    [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */},
                                    text));

    // LOG(info) << "Sending \"" << text << "\"";

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
    delete static_cast<char*>(text);
}