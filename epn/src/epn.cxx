#include "epn.h"

using namespace std;

EventProccessingNode::EventProccessingNode()
    : fNumFlp(0)
    , receivedMessages(0)
{
    // register a handler for data arriving on "data" channel
    OnData("1", &EventProccessingNode::HandleData);
}

void EventProccessingNode::InitTask()
{
    fNumFlp = fConfig->GetValue<uint64_t>("num-flp");
}

// handler is called whenever a message arrives on "data", with a reference to the message and a sub-channel index (here 0)
bool EventProccessingNode::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
    // LOG(info) << "Received: \"" << string(static_cast<char*>(msg->GetData()), msg->GetSize()) << "\"";
    receivedMessages++;
    if(receivedMessages >= fNumFlp) {
        receivedMessages = 0;
        FairMQMessagePtr msg(NewSimpleMessage(fConfig->GetValue<std::string>("id")));
        // in case of error or transfer interruption, return false to go to IDLE state
        // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
        // LOG(error) << fConfig->GetValue<std::string>("id");
        Send(msg, "feedback");
    }
    // return true if want to be called again (otherwise return false go to IDLE state)
    return true;
}

EventProccessingNode::~EventProccessingNode()
{
}