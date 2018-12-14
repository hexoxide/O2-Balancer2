#include "icn.h"

#include <random>

#include "o2data.h"

using namespace std;

InformationControlNode::InformationControlNode()
	: fIterations(0)
	, numHeartbeat(0)
	, channels()
	, isConfigure(true)
	, startTime()
{
}

void InformationControlNode::InitTask()
{
	fIterations = fConfig->GetValue<uint64_t>("iterations");
}

void InformationControlNode::PreRun()
{
	feedbackListener = thread(&InformationControlNode::ListenForFeedback, this);
}

bool InformationControlNode::ConditionalRun()
{
	FairMQParts parts;

	// Send an intial dummy packet to setup all pub-sub connections
	if(isConfigure) {
		LOG(trace) << "Setting up broadcast channel";
		FairMQParts firstParts;
		O2Data* firstPacket = new O2Data();
		firstPacket->heartbeat = numHeartbeat;
		firstPacket->tarChannel = 0;
		firstPacket->configure = false;
		void* dataFirst = firstPacket;
    	firstParts.AddPart(NewMessage(dataFirst, 
    					sizeof(O2Data),
						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                        firstPacket));
    	Send(firstParts, "broadcast", 0, 0); // Send the packet asynchronously
    	LOG(trace) << "Waiting for FLP's to register on broadcast channel";
    	this_thread::sleep_for(chrono::seconds(5)); // Wait 10 seconds to ensure all pub-sub channels are setup
    	LOG(trace) << "FLP's should have registered?";
    }

    // First part of message should always be of type O2Data
	O2Data* s1 = new O2Data();
	s1->heartbeat = numHeartbeat;
	//mt19937 rng;
    //rng.seed(random_device()());
    //uniform_int_distribution<mt19937::result_type> dist4(1,1);
    // TODO round-robin algorithm
	s1->tarChannel = 1; //dist4(rng); // selected channel for flp's to transmit on
	s1->configure = isConfigure; // if the packet is configuring the flp channels
	void* data1 = s1;
    parts.AddPart(NewMessage(data1, 
    						sizeof(O2Data),
    						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                            s1));

    // TODO configure based on packets from EPN's
	if(isConfigure) {
	    for (uint8_t i = 1; i < 2 /*UINT8_MAX*/; i++) {
		    O2Channel* s2 = new O2Channel();
			s2->index = i;
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

	// Send messages to FLP's as long as fIterations is not reached
	if(numHeartbeat < fIterations) 
	{
    	Send(parts, "broadcast", 0, 0);
    	if(isConfigure)
    	{
    		this_thread::sleep_for(chrono::seconds(2));
    		startTime = chrono::system_clock::now();
    	}
    	else {
    		numHeartbeat++;
    	}
    }
    else
    {
    	LOG(trace) << "Done sending packets";
    	LOG(trace) << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count();
    	this_thread::sleep_for(chrono::seconds(1));
    	return false;
    }

    // only the first packet should configure the flp
    isConfigure = false;

	return true;
}

void InformationControlNode::PostRun()
{
	LOG(trace) << "Heartsbeats	" << numHeartbeat;
	feedbackListener.join();
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