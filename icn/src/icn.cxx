#include "icn.h"

#include <random>
#include "o2data.h"

using namespace std;

/**
 *
 * Completion function invoked when the call to get
 * the list of tasks returns.
 *
 */
void epn_completion (int rc,
					const struct String_vector *strings,
					const void *data) {
	//struct String_vector *tmp_tasks;
	switch (rc) {
		case ZCONNECTIONLOSS:
		case ZOPERATIONTIMEOUT:
		{
			get_epns();
		}
		break;
		case ZOK:
		{
			printf("Assigning epns\n");
			// struct String_vector *tmp_tasks = added_and_set(strings, &epns);
			//assign_tasks(strings);
			for(int i = 0; i < strings->count; i++) {
				printf("%s", strings->data[i]);
			}
			//free_vector(tmp_tasks);
		}
		break;
		default:
			printf("Something went wrong when checking tasks: %d", rc);

			break;
	}
}
//asynch retrieev epn and place watcher
void get_epns () {
	printf("Getting tasks\n");
		zoo_awget_children(zh,
						"/EPN",
						epn_watcher,
						NULL,
						epn_completion,
						NULL);
}

InformationControlNode::InformationControlNode()
	: fIterations(0)
	, stateChangeHook("hook")
    , currentReconfigureStep(0)
	, numHeartbeat(0)
	, feedbackListener()
	, channels()
	, isConfigure(true)
	, startTime()
{
	char buffer[512];
	printf("starting program\n");
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
		return errno;
	}

	//  struct ACL CREATE_ONLY_ACL[] = {{ZOO_PERM_CREATE, ZOO_AUTH_IDS}};
	//  struct ACL_vector CREATE_ONLY = {1, CREATE_ONLY_ACL};
	int rc = zoo_create(zh,"/xyz","value", 5, &ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE,
						buffer, sizeof(buffer)-1);

	/** this operation will fail with a ZNOAUTH error */
	int buflen= sizeof(buffer);
	struct Stat stat;
	rc = zoo_get(zh, "/xyz0000000006", 0, buffer, &buflen, &stat);
	if (rc) {
		fprintf(stderr, "Error %d \n", rc);
	}
	if (rc == ZOK) {
		std::string log = std::string(buffer, static_cast<unsigned long>(512));
		printf("opgehaalde value: %s\n", log.c_str());
	}
	//get_epns();
	// zookeeper_close(zh);
	// return 0;	
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
    	this_thread::sleep_for(chrono::seconds(10)); // Wait 10 seconds to ensure all pub-sub channels are setup
    	LOG(trace) << "FLP's should have registered?";
    }

    // First part of message should always be of type O2Data
	O2Data* s1 = new O2Data();
	s1->heartbeat = numHeartbeat;
	mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<mt19937::result_type> dist4(1,1);
	s1->tarChannel = dist4(rng); // selected channel for flp's to transmit on
	s1->configure = isConfigure; // if the packet is configuring the flp channels
	void* data1 = s1;
    parts.AddPart(NewMessage(data1, 
    						sizeof(O2Data),
    						[](void* /*data*/, void* object) { delete static_cast<O2Data*>(object); },
                            s1));

    // ToDo 
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
    	this_thread::sleep_for(chrono::milliseconds(20));
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
	//SubscribeToStateChange(stateChangeHook, ExitDevice);
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