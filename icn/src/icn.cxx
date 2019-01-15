#include "icn.h"

#include <random>
#include "o2data.h"

#include <string.h>
#include <errno.h>
#include <string>
#include "zookeeper/zookeeper.h"
#include <unistd.h>
#include <stdarg.h>
#include <iostream>

using namespace std;

std::unique_ptr<char[]> InformationControlNode::text = nullptr;

InformationControlNode::InformationControlNode()
	: fIterations(0)
	, stateChangeHook("hook")
    , currentReconfigureStep(0)
	, numHeartbeat(0)
	, channels()
	, isConfigure(true)
	, startTime()
{
	
	
}

void InformationControlNode::InitTask()
{
	fIterations = fConfig->GetValue<uint64_t>("iterations");
	fIterations = fConfig->GetValue<uint64_t>("rate");
	startTime = chrono::high_resolution_clock::now();
    text = unique_ptr<char[]>(new char[1]);
}

bool InformationControlNode::ConditionalRun()
{	
    auto nowTime   = chrono::high_resolution_clock::now();
    auto mseconds = chrono::duration_cast<chrono::milliseconds>(nowTime - startTime).count();

    std::cout << "millis: " << mseconds;
	if(mseconds > 1000 && numHeartbeat < fIterations) 
	{
        FairMQMessagePtr msgToSend(NewMessage(text.get(),
                                    1,
                                    [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */ },
                                    text.get()));
    	Send(msgToSend, "broadcast", 0, 0);
		numHeartbeat++;
		return true;
    }
    else
    {
    	LOG(trace) << "Done sending packets";
    	LOG(trace) << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count();
    	this_thread::sleep_for(chrono::milliseconds(20));
    	return false;
    }
	return true;
}

void InformationControlNode::PostRun()
{
	LOG(trace) << "Heartsbeats	" << numHeartbeat;
	//SubscribeToStateChange(stateChangeHook, ExitDevice);
}

void InformationControlNode::ExitDevice(const State curState)
{
	LOG(trace) << "Breaking down";
    if(curState == READY && currentReconfigureStep == 1)
    {
        currentReconfigureStep = 2;
        LOG(trace) << "DOWN step 1 - Current state is ready";
        ChangeState("RESET_TASK");
        // WaitForEndOfStateForMs("RESET_TASK", 1);
    }
    // Step 2
    if(curState == DEVICE_READY && currentReconfigureStep == 2)
    {
        currentReconfigureStep = 3;
        LOG(trace) << "DOWN step 2 - Current state is device ready";
        ChangeState("RESET_DEVICE");
        //WaitForEndOfStateForMs("RESET_DEVICE", 1);
    }
    // Step 3
    if(curState == IDLE && currentReconfigureStep == 3)
    {
        currentReconfigureStep = 4;
        LOG(trace) << "DOWN step 3 - Current state is idle";
        ChangeState("END");
        // WaitForEndOfStateForMs("INIT_DEVICE", 1);
    }
}

InformationControlNode::~InformationControlNode()
{
}
