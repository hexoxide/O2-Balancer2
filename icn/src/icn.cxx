#include "icn.h"

#include "FairMQPoller.h"

using namespace std;

InformationControlNode::InformationControlNode()
	: fFeedbackListener()
{
}

void InformationControlNode::InitTask()
{
	//poller = FairMQPollerPtr(NewPoller("broadcast", "feedback"));
	//poller->Poll(fConfig->GetValue<float>("rate") * 10);
}

void InformationControlNode::PreRun()
{
	fFeedbackListener = thread(&InformationControlNode::ListenForFeedback, this);
}

bool InformationControlNode::ConditionalRun()
{
	//while(CheckCurrentState(RUNNING)) {
		// if (poller->CheckInput("feedback", 0)) {
		// 	LOG(info) << "Received Feedback";
		// 	FairMQMessagePtr msgFeedback(NewMessage());
		// 	Receive(msgFeedback, "feedback");
		// }

		//if (poller->CheckOutput("broadcast", 0)) {
			// Create a message containing
		    FairMQMessagePtr msg(NewSimpleMessage("1"));

		    // in case of error or transfer interruption, return false to go to IDLE state
		    // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
		    Send(msg, "broadcast");
		//}
	//}

	return true;
}

void InformationControlNode::PostRun()
{
	fFeedbackListener.join();
	//changeState(RUNNING);
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

    LOG(info) << "Acknowledged " << numAcks << " messages";
}

InformationControlNode::~InformationControlNode()
{
}