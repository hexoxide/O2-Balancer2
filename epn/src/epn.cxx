#include "epn.h"

#include "linux-interface.h"
#include "o2exception.h"
#include "data/o2data.h"
#include "data/o2channel.h"

using namespace std;
using namespace O2::data;
using namespace O2::exception;

/** Name of the channel an epn will return information about received messages on */
const std::string EventProcessingNode::feedbackChannel = "feedback";

/** Number of FLP's the EPN will expect to be in the network */
const std::string EventProcessingNode::pNumberOfFLP = "num-flp";

/** The primary network interface to use while performing load balancing experiments */
const std::string EventProcessingNode::pPrimaryNetworkInterface = "primary-interface";

EventProcessingNode::EventProcessingNode()
    : initialDelay(10)
    , fNumFlp(0)
    , fPrimaryNetworkInterface("")
    , receivedMessages(0)
    , currentHeartbeat(0)
    , isCurrentlyOutOfOrder(false)
{
}

void EventProcessingNode::InitTask()
{
    // register a handler for data arriving on "data" channel
    OnData(fConfig->GetValue<std::string>("id"), &EventProcessingNode::HandleData);
    fNumFlp = fConfig->GetValue<uint64_t>(pNumberOfFLP);
    fPrimaryNetworkInterface = fConfig->GetValue<std::string>(pPrimaryNetworkInterface);
}

void EventProcessingNode::PreRun()
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
    /* Get interface ipv4 address for the specific network interface*/
    auto  s2 = new O2Channel(O2::network::getInterfaceAddress(fPrimaryNetworkInterface));
    try {
        s2->index = stoll(fConfig->GetValue<std::string>("id"));
    }
    catch(std::exception& e) {
        throw O2Exception(e.what(), __FILE__, __LINE__);
    }
    // TODO update this according to the configured channel information were the id matches the channel name
    s2->port = 5555;
    LOG(TRACE) << "EPN will send the following channel confguration to ICN: " << std::string(*s2);
    void* data2 = s2;
    parts.AddPart(NewMessage(data2, 
                            sizeof(O2Channel),
                            [](void* /*data*/, void* object) { delete static_cast<O2Channel*>(object); },
                            s2));
    this_thread::sleep_for(chrono::seconds(initialDelay));
    LOG(TRACE) << "Sending configuration packet";
    Send(parts, feedbackChannel, 0, 0); // Send the packet asynchronously
}

/**
 * Receives data from the FLP and determines if they were received in order and if an acknowledgement should be send.
 * @param msg the data send by the FLP broken up into individual parts.
 * @return if the handling of the data was successful
 */
bool EventProcessingNode::HandleData(FairMQParts& msg, int index)
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

EventProcessingNode::~EventProcessingNode()
= default;
