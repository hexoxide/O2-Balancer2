#include "icn.h"

#include "FairMQPoller.h"

using namespace std;

InformationControlNode::InformationControlNode()
{
}

void InformationControlNode::InitTask()
{
	poller = FairMQPollerPtr(NewPoller("broadcast", "feedback"));
	poller->Poll(fConfig->GetValue<float>("rate"));
}

bool InformationControlNode::ConditionalRun()
{
	if (poller->CheckInput("feedback", 0)) {

	}

	if (poller->CheckOutput("broadcast", 0)) {
		// Create a message containing
	    FairMQMessagePtr msg(NewSimpleMessage("1"));

	    // in case of error or transfer interruption, return false to go to IDLE state
	    // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
	    Send(msg, "broadcast");
	}

	return true;
}

void InformationControlNode::PostRun()
{
	ChangeState(RUNNING);
}

InformationControlNode::~InformationControlNode()
{
}