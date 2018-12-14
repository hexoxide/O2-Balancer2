#include "epn.h"

#include "o2data.h"
#include "o2channel.h"

using namespace std;

/** Name of the channel an epn will return information about received messages on */
const std::string EventProccessingNode::feedbackChannel = "feedback";

/** Number of FLP command line argument, used to set fNumFlp variable */
const std::string EventProccessingNode::pNumberOfFLP = "num-flp";

EventProccessingNode::EventProccessingNode()
    : fNumFlp(0)
    , receivedMessages(0)
    , currentHeartbeat(0)
    , isCurrentlyOutOfOrder(false)
{
}

void EventProccessingNode::InitTask()
{
    // register a handler for data arriving on "data" channel
    OnData(fConfig->GetValue<std::string>("id"), &EventProccessingNode::HandleData);
    fNumFlp = fConfig->GetValue<uint64_t>(pNumberOfFLP);
}

/**
 * s
 * @return if the handling of the data was successful
 */
bool EventProccessingNode::HandleData(FairMQParts& msg, int /*index*/)
{
    bool firstMessagePart = true;
    O2Data* data;

    // Iterate through all message parts
    for (const auto& part : msg)
    {
        if(firstMessagePart)
        {
            firstMessagePart = false;
            data = static_cast<O2Data*>(part->GetData());
        }
    }

    // When the first message / sub-timeframe arrives register its heartbeat id.
    if(receivedMessages == 0 && currentHeartbeat < data->heartbeat)
    {
        currentHeartbeat = data->heartbeat;
    }

    // Validate the current message heartbeat is equal to the heartbeat we should currently be receiving from FLP's
    receivedMessages++;
    if(currentHeartbeat != data->heartbeat && !isCurrentlyOutOfOrder)
    {
        // LOG(TRACE) << "Message " << to_string(receivedMessages) << " arrived out of order!";
        isCurrentlyOutOfOrder = true;
    }

    // if more messages than FLP's present have been received send a feedback message
    if(receivedMessages >= fNumFlp) 
    {
        receivedMessages = 0;

        // Before sending the feedback ensure the current set of messages wasn't out of order
        if(!isCurrentlyOutOfOrder) {
            FairMQMessagePtr feedback(NewSimpleMessage(fConfig->GetValue<std::string>("id")));
            Send(feedback, feedbackChannel, 0, 0);
        }

        isCurrentlyOutOfOrder = false;
    }

    return true;
}

EventProccessingNode::~EventProccessingNode()
{
}