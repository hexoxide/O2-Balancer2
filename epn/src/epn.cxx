#include "epn.h"

#include "data/o2data.h"
#include "data/o2channel.h"

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

void EventProccessingNode::PreRun()
{
    FairMQParts parts;
    auto  firstPacket = new O2Data();
    firstPacket->heartbeat = 0;
    firstPacket->tarChannel = 0;
    firstPacket->configure = true;
    void* data1 = firstPacket;
    parts.AddPart(NewMessage(data1,
                            sizeof(O2Data),
                            [](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                            firstPacket));
    // TODO ask kernel nicely for ip address try not to use unreadable C examples.
    auto  s2 = new O2Channel();
    s2->index = stoll(fConfig->GetValue<std::string>("id"));
    s2->ip1 = 127;
    s2->ip2 = 0;
    s2->ip3 = 0;
    s2->ip4 = 1;
    s2->port = 5555;
    void* data2 = s2;
    parts.AddPart(NewMessage(data2, 
                            sizeof(O2Channel),
                            [](void* /*data*/, void* object) { delete static_cast<O2Channel*>(object); },
                            s2));
    this_thread::sleep_for(chrono::seconds(10));
    LOG(TRACE) << "Sending configuration packet";
    Send(parts, feedbackChannel, 0, 0); // Send the packet asynchronously
}

/**
 * s
 * @return if the handling of the data was successful
 */
bool EventProccessingNode::HandleData(FairMQParts& msg, int /*index*/)
{
    bool firstMessagePart = true;
    O2Data* data = nullptr;

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
= default;
