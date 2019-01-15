#include "icn.h"

#include <random>

#include "data/o2data.h"

using namespace std;
using namespace O2::data;

InformationControlNode::InformationControlNode()
	: initialDelay(10)
	, fIterations(0)
	, numHeartbeat(0)
	, channels()
	, currentChannel(nullptr)
	, isConfigure(true)
	, isPreConfigure(true)
	, startTime()
{
}

/**
 * @return Number of established channels.
 */ 
uint64_t InformationControlNode::getNumberOfChannels()
{
	return channels.size();
}

void InformationControlNode::InitTask()
{
	fIterations = fConfig->GetValue<uint64_t>("iterations");
	// OnData("feedback", &InformationControlNode::HandleFeedback);
}

/**
 * Does additional actions before the experiment.
 */
void InformationControlNode::PreRun()
{
	feedbackListener = thread(&InformationControlNode::ListenForFeedback, this);
}

/**
 * Starts monitoring the experiment.
 */
bool InformationControlNode::ConditionalRun()
{
	FairMQParts parts;

	// Wait until isPreConfigure is done
	if(isPreConfigure) {
		return true;
	}

	// Send an intial dummy packet to setup all pub-sub connections
	if(isConfigure) {
		//LOG(trace) << "Setting up broadcast channel";
		//FairMQParts firstParts;
		//O2Data* firstPacket = new O2Data();
		//firstPacket->heartbeat = numHeartbeat;
		//firstPacket->tarChannel = 1;
		//firstPacket->configure = false;
		//void* dataFirst = firstPacket;
    	//firstParts.AddPart(NewMessage(dataFirst, 
    	//				sizeof(O2Data),
		//				[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
        //               firstPacket));
    	//Send(firstParts, "broadcast", 0, 0); // Send the packet asynchronously
    	LOG(trace) << "Waiting for FLP's to register on broadcast channel";
    	this_thread::sleep_for(chrono::seconds(initialDelay)); // Wait 10 seconds to ensure all pub-sub channels are setup
    	LOG(trace) << "FLP's should have registered?";
    }

    // First part of message should always be of type O2Data
	auto  s1 = new O2Data();
	s1->heartbeat = numHeartbeat;
	//mt19937 rng;
    //rng.seed(random_device()());
    //uniform_int_distribution<mt19937::result_type> dist4(1,1);
    // TODO round-robin algorithm
	s1->tarChannel = determineChannel(); // selected channel for flp's to transmit on
	s1->configure = isConfigure; // if the packet is configuring the flp channels
	void* data1 = s1;
    parts.AddPart(NewMessage(data1, 
    						sizeof(O2Data),
    						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                            s1));

    // TODO configure based on packets from EPN's
	if(isConfigure) {
	    for (auto & channel : channels) {
		    // O2Channel* s2 = new O2Channel();
			// s2->index = i;
			// s2->ip1 = 127;
			// s2->ip2 = 0;
			// s2->ip3 = 0;
			// s2->ip4 = 1;
			// s2->port = 5555;
			void* data = channel;
		    parts.AddPart(NewMessage(data, 
		                            sizeof(O2Channel),
		                            [](void* /*data*/, void* object) {
		                                // delete static_cast<O2Channel*>(object);
		                            },
		                            channel));
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

/**
 * Does additional action after the experiment, Gives 
 * information about the experiment through logs and 
 * closes listeners.
 */
void InformationControlNode::PostRun()
{
	LOG(trace) << "Heartsbeats	" << numHeartbeat;
	feedbackListener.join();
}

/**
 * Determines the next appropriate channel the FLP should use 
 * to send data to the EPN.
 * @return The desired channel.
 */
uint64_t InformationControlNode::determineChannel()
{
	uint64_t index = 0;

	// currentChannel initialization 
	if(currentChannel == nullptr)
	{
		currentChannel = channels.at(0);
	}

	if(currentChannel == channels.back())
	{
		// LOG(trace) << "current channel was last entry";
		currentChannel = channels.front();
	}
	else 
	{
		// LOG(trace) << "Advancing channel";
		auto it = std::find(channels.begin(), channels.end(), currentChannel);
		std::advance (it, 1);
		index = std::distance(channels.begin(), it);
		currentChannel = channels.at(index);
	}

	// LOG(trace) << "Chosen channel:" << currentChannel->index;
	return currentChannel->index;
}

/**
 * Counts the number of feedbacks from EPN's. This can be used 
 * to determine the number of lost heartbeats during the 
 * experiment.
 */
void InformationControlNode::ListenForFeedback()
{
    uint64_t numAcks = 0;
    while (CheckCurrentState(RUNNING))
    {
        FairMQParts msg;
        // If we have no messages do nothing
        if (Receive(msg, "feedback") < 0) break;

        if(isConfigure) {
        	// after first channel packet leave preConfigure
        	// this will wait additional 10 seconds before leaving isConfigure which will ignore further channel messages
        	isPreConfigure = false;
        	// Iterate through all parts, first part is always of type O2Data so we ignore it
	    	bool isFirst = true;
		    for (const auto& part : msg) {
		    	if(isFirst) 
	    		{
	    			isFirst = false;
	    			continue;
	    		}
	    		// Copy data and assign to pointer
	    		// TODO std::unique_ptr
		    	auto  data = new O2Channel(*static_cast<O2Channel*>(part->GetData()));
		    	LOG(TRACE) << "Got channel " << std::string(*data);
		    	// Push data into vector
		    	channels.push_back(data);
		    }
        }
        // TODO verify the EPN from which the ack was received and its heartbeat
        else {
        	++numAcks;
        }
    }
    LOG(trace) << "Acknowledgements	" << numAcks;
}

// bool InformationControlNode::HandleFeedback(FairMQParts& msg, int index)
// {
// 	// If this is the first feedback packet we leave the preconfigure state
// 	if(isPreConfigure) {
//     	isPreConfigure = false;
//     	LOG(TRACE) << "Received EPN channel packets, leaving preConfigure state";

// 	    // Iterate through all parts, first part is always of type O2Data so we ignore it
// 	    bool isFirst = true;
// 	    for (const auto& part : msg) {
// 	    	if(isFirst) 
//     		{
//     			isFirst = false;
//     			continue;
//     		}
//     		// Copy data and assign to pointer
//     		// TODO std::unique_ptr
    		
// 	    	O2Channel* data = new O2Channel(*static_cast<O2Channel*>(part->GetData()));
// 	    	LOG(TRACE) << "Got channel " << to_string(data->index) << " " << to_string(data->port);
// 	    	// Push data into vector
// 	    	channels.push_back(data);
// 	    }
//     }
// 	return true;
// }

InformationControlNode::~InformationControlNode()
= default;