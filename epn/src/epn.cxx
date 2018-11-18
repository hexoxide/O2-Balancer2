#include "epn.h"

using namespace std;

EventProccessingNode::EventProccessingNode()
{
    // register a handler for data arriving on "data" channel
    OnData("1", &EventProccessingNode::HandleData);
}

void EventProccessingNode::InitTask()
{
}

// handler is called whenever a message arrives on "data", with a reference to the message and a sub-channel index (here 0)
bool EventProccessingNode::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
    // LOG(info) << "Received: \"" << string(static_cast<char*>(msg->GetData()), msg->GetSize()) << "\"";

    // return true if want to be called again (otherwise return false go to IDLE state)
    return true;
}

EventProccessingNode::~EventProccessingNode()
{
}