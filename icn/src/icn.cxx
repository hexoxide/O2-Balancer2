#include "icn.h"

#include <random>
#include "o2data.h"

using namespace std;

InformationControlNode::InformationControlNode()
	: fFeedbackListener()
	, fNumHeartbeat(0)
	, channels()
	, isConfigure(true)
{
}

void InformationControlNode::InitTask()
{
}

void InformationControlNode::PreRun()
{
	fFeedbackListener = thread(&InformationControlNode::ListenForFeedback, this);
}

bool InformationControlNode::ConditionalRun()
{
	fNumHeartbeat++;
	FairMQParts parts;

	// Send an intial dummy packet to setup all pub-sub connections
	if(isConfigure) {
		LOG(trace) << "Setting up broadcast channel";
		FairMQParts firstParts;
		O2Data* firstPacket = new O2Data();
		firstPacket->heartbeat = fNumHeartbeat;
		firstPacket->tarChannel = 0;
		firstPacket->configure = false;
		void* dataFirst = firstPacket;
    	firstParts.AddPart(NewMessage(dataFirst, 
    					sizeof(O2Data),
						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                        firstPacket));
    	Send(firstParts, "broadcast", 0, 0); // Send the packet asynchronously
    	LOG(trace) << "Waiting for FLP's to register on broadcast channel";
    	std::this_thread::sleep_for(std::chrono::seconds(10)); // Wait 10 seconds to ensure all pub-sub channels are setup
    	LOG(trace) << "FLP's should have registered?";
    }

    // First part of message should always be of type O2Data
	O2Data* s1 = new O2Data();
	s1->heartbeat = fNumHeartbeat;
	mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<mt19937::result_type> dist4(1,4);
	s1->tarChannel = dist4(rng); // selected channel for flp's to transmit on
	s1->configure = isConfigure; // if the packet is configuring the flp channels
	void* data1 = s1;
    parts.AddPart(NewMessage(data1, 
    						sizeof(O2Data),
    						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                            s1));

	if(isConfigure) {
	    for (uint8_t i = 1; i < 5 /*UINT8_MAX*/; i++) {
		    O2Channel* s2 = new O2Channel();
			s2->index = i;
			//s2->ip1 = 192;
			//s2->ip2 = 168;
			//s2->ip3 = 0;
			//s2->ip4 = i;
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
		}
	}

    Send(parts, "broadcast");

    // only the first packet should configure the flp
    isConfigure = false;

	return true;
}

void InformationControlNode::PostRun()
{
	LOG(trace) << "Heartsbeats	" << fNumHeartbeat;
	fFeedbackListener.join();
}

/**
 * Count the number of feedbacks from EPN's 
 * this can be used to determine the number of lost heartbeats
 */
void InformationControlNode::ListenForFeedback()
{
    uint64_t numAcks = 0;
    while (CheckCurrentState(RUNNING))
    {
        FairMQMessagePtr ack(NewMessageFor("feedback", 0));
        if (Receive(ack, "feedback") < 0)
        {
            break;
        }
        ++numAcks;
    }
    LOG(trace) << "Acknowledgements	" << numAcks;
}

InformationControlNode::~InformationControlNode()
{
}