#include "icn.h"

#include "o2data.h"
#include "o2channel.h"

using namespace std;

InformationControlNode::InformationControlNode()
	: fFeedbackListener()
	, fNumHeartbeat(0)
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

	if(isConfigure) {
		LOG(error) << "FirstPacket";
		FairMQParts firstParts;
		O2Data* firstPacket = new O2Data();
		firstPacket->heartbeat = fNumHeartbeat;
		firstPacket->tarChannel = 1;
		firstPacket->configure = false;
		void* dataFirst = firstPacket;
    	firstParts.AddPart(NewMessage(dataFirst, 
    					sizeof(O2Data),
						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                        firstPacket));
    	Send(firstParts, "broadcast");
    }

	O2Data* s1 = new O2Data();
	s1->heartbeat = fNumHeartbeat;
	s1->tarChannel = 1; // selected channel for flp's to transmit on
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
			s2->ip1 = 192;
			s2->ip2 = 168;
			s2->ip3 = 0;
			s2->ip4 = i;
			s2->port = 5000 + i;
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
    LOG(error) << "isConfigure false";

	return true;
}

void InformationControlNode::PostRun()
{
	LOG(info) << "Heartsbeats	" << fNumHeartbeat;
	fFeedbackListener.join();
}

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
    LOG(info) << "Acknowledgements	" << numAcks;
}

InformationControlNode::~InformationControlNode()
{
}