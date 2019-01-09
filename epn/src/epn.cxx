#include "epn.h"

#include <string.h>
#include <errno.h>
#include <string>
#include "zookeeper/zookeeper.h"
#include <unistd.h>
#include <stdarg.h>

using namespace std;

static zhandle_t *zh;
int connected = 0;
int expired = 0;
static struct String_vector* epns = NULL;

/** main Watcher function , cotains events for connecting status */
void watcher(zhandle_t *zzh,
              int type,
              int state,
              const char *path,
              void *watcherCtx)
  {
    if (type == ZOO_SESSION_EVENT) {
       if (state == ZOO_CONNECTED_STATE) {
           connected = 1;

           printf("Received a connected event.\n");
       } else if (state == ZOO_CONNECTING_STATE) {
           if(connected == 1) {
               printf("Disconnected.\n");
           }
           connected = 0;
       } else if (state == ZOO_EXPIRED_SESSION_STATE) {
           expired = 1;
           connected = 0;
           zookeeper_close(zzh);
       }
    }
}

EventProccessingNode::EventProccessingNode()
    : fNumFlp(0)
    , address("")
    , receivedMessages(0)
{
    // register a handler for data arriving on "data" channel
    OnData("1", &EventProccessingNode::HandleData);
	printf("starting program\n");
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

}

void EventProccessingNode::InitTask()
{
    //address = fConfig->GetValue<string>("address");
    fNumFlp = fConfig->GetValue<uint64_t>("num-flp");
    char buffer[512];

	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
		//return errno;
	}
    std::string address = GetChannel("1").GetAddress();

	int rc = zoo_create(zh,"/EPN/", address.c_str(), strlen(address), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE,
						buffer, sizeof(buffer)-1);
}

// handler is called whenever a message arrives on "data", with a reference to the message and a sub-channel index (here 0)
bool EventProccessingNode::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
    LOG(info) << "Received: \"" << string(static_cast<char*>(msg->GetData()), msg->GetSize()) << "\"";
    // receivedMessages++;
    // if(receivedMessages >= fNumFlp) {
    //     receivedMessages = 0;
    //     FairMQMessagePtr msg(NewSimpleMessage(fConfig->GetValue<std::string>("id")));
    //     // in case of error or transfer interruption, return false to go to IDLE state
    //     // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
    //     // LOG(error) << fConfig->GetValue<std::string>("id");
    //     Send(msg, "feedback");
    // }
    // return true if want to be called again (otherwise return false go to IDLE state)
    return true;
}

EventProccessingNode::~EventProccessingNode()
{
}
