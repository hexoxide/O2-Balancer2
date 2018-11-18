#include "icn.h"

using namespace std;

InformationControlNode::InformationControlNode()
{
}

void InformationControlNode::InitTask()
{
}

bool InformationControlNode::ConditionalRun()
{
    // create message object with a pointer to the data buffer,
    // its size,
    // custom deletion function (called when transfer is done),
    // and pointer to the object managing the data buffer
    FairMQMessagePtr msg(NewSimpleMessage("1"));

    // in case of error or transfer interruption, return false to go to IDLE state
    // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
    if (Send(msg, "broadcast") < 0)
    {
        return false;
    }
    return true;
}

InformationControlNode::~InformationControlNode()
{
}