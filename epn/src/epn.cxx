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
            LOG (info) << "Received a connected event.";
       } else if (state == ZOO_CONNECTING_STATE) {
           if(connected == 1) {
               LOG (error) << "zookeeper disconnected";
           }
           connected = 0;
       } else if (state == ZOO_EXPIRED_SESSION_STATE) {
           expired = 1;
           connected = 0;
           zookeeper_close(zzh);
               LOG (error) << "zookeeper expired";
       }
    }
}

EventProccessingNode::EventProccessingNode()
    : address("")
    , receivedMessages(0)
{
    // register a handler for data arriving on "data" channel
    OnData("1", &EventProccessingNode::HandleData);
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

}

void EventProccessingNode::InitTask()
{
    char buffer[512];

	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
		//return errno;
	}
    
    std::string address = GetChannel("1").GetAddress();
	int rc = zoo_create(zh,"/EPN/", address.c_str(), address.length(), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE,
						buffer, sizeof(buffer)-1);
}

// handler is called whenever a message arrives on "data", with a reference to the message and a sub-channel index (here 0)
bool EventProccessingNode::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
    //LOG(info) << "Received message! length: " << to_string(msg->GetSize());
    return true;
}

EventProccessingNode::~EventProccessingNode()
{
}
