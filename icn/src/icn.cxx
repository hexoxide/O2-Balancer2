#include "icn.h"

#include "o2data.h"
#include "o2channel.h"

using namespace std;

InformationControlNode::InformationControlNode()
	: fFeedbackListener()
	, fNumHeartbeat(0)
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

	O2Data* s1 = new O2Data();
	s1->heartbeat = fNumHeartbeat;
	s1->tarChannel = 1;
	s1->configure = true;
	void* data1 = s1;
    parts.AddPart(NewMessage(data1, 
    						sizeof(O2Data),
    						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                            s1));

    for (uint8_t i = 1; i < 3 /*UINT8_MAX*/; i++) {
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
    // in case of error or transfer interruption, return false to go to IDLE state
    // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
    Send(parts, "broadcast");

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