#include "flp.h"

#include "o2data.h"
#include "o2channel.h"

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
           LOG(trace) << "Received a connected event.";
       } else if (state == ZOO_CONNECTING_STATE) {
           if(connected == 1) {
               LOG(trace) << "DIsconnected";
           }
           connected = 0;
       } else if (state == ZOO_EXPIRED_SESSION_STATE) {
           expired = 1;
           connected = 0;
           zookeeper_close(zzh);
       }
    }
}
char * make_path(int num, ...) {
    const char * tmp_string;

    va_list arguments;
    va_start ( arguments, num );

    int total_length = 0;
    int x;
    for ( x = 0; x < num; x++ ) {
        tmp_string = va_arg ( arguments, const char * );
        if(tmp_string != NULL) {
            //LOG_DEBUG(("Counting path with this path %s (%d)", tmp_string, num));
            total_length += strlen(tmp_string);
        }
    }

    va_end ( arguments );

    char * path = (char*)malloc(total_length * sizeof(char) + 1);
    path[0] = '\0';
    va_start ( arguments, num );

    for ( x = 0; x < num; x++ ) {
        tmp_string = va_arg ( arguments, const char * );
        if(tmp_string != NULL) {
            //LOG_DEBUG(("Counting path with this path %s",tmp_string));
            strcat(path, tmp_string);
        }
    }

    return path;
}

void FirstLineProccessing::get_epn_data_completion(int rc, const char *value, int value_len,
                              const struct Stat *stat, const void *data) {
    //int worker_index;

    switch (rc) {
        case ZCONNECTIONLOSS:
        case ZOPERATIONTIMEOUT:
            get_epn_data((const char *) data);

            break;

        case ZOK:
        {
            char* nodeName = (char *) data;
            char* nodeValue = strndup(value, value_len);
            isReconfiguringChannels = true;
            epnsListChanged = true; 
            LOG(trace) << "Configuring";
            LOG(trace) << "nodename: " << nodeName;
            LOG(trace) << "nodeValue: " << nodeValue;

            int epnID = std::stoi (nodeName);
            listOfAvailableEpns[epnID] = nodeValue;

            listOfNewEpns[epnID] = nodeValue;
            numberOfNewEpnsRetrieved += 1;
        }
            break;
        // default:
        //     //LOG_ERROR(("Something went wrong when checking the master lock: %s",rc2string(rc)));
        //     break;
    }
}
void FirstLineProccessing::get_epn_data(const char *epn) {
    if(epn == NULL) return;
    char * epnName = strndup(epn, 15);
    char * path = make_path(2, "/EPN/", epnName);
    //LOG_DEBUG(("Getting epn data %s",epnName));
    //data_completion_t get_epn_data_completion_bound = <decltype( std::bind(&FirstLineProccessing::get_epn_data_completion, this, _1, _2, _3, _4, _5) )>();
    
    zoo_aget(zh,
             path,
             0,
             get_epn_data_completion,
             (const void *) epnName);
    free(path);
    //free(epnName); // dit sloopt eht op de een of andere manier, terwijl het wel moet volgens c++ reference na strndup
}
void FirstLineProccessing::handleZkEpnsUpdate(const struct String_vector *strings) {
    int amountZkEpns = strings->count;
    //LOG_DEBUG(("epn count: %d", strings->count));
    if( amountZkEpns > listOfAvailableEpns.size()){
        //zk inserted new epn
        int i;
        for( i = 0; i < amountZkEpns; i++) {
            std::map<int, std::string>::iterator iterator = listOfAvailableEpns.find(atoi(strings->data[i]));
            if (iterator == listOfAvailableEpns.end()){
                LOG(trace) << "new node!";
                numberOfNewEpns += 1;
                get_epn_data( strings->data[i] );
                //doesnt have to check if the channel already existed because its ephenumeral
            }
        }
        epnsChanged = true;
    }else if(amountZkEpns < listOfAvailableEpns.size()){
        //zk deleted epn
        bool foundEpn;
        epnsListChanged = true; //needed because this is a different thread, and it might cause a seg fault in run
        std::map<int, std::string>::iterator it;
        for (it=listOfAvailableEpns.begin(); it!=listOfAvailableEpns.end(); ++it){
            LOG(trace) << "iterating: " << it->second;
            int i;
            foundEpn = false;
            for( i = 0; i < amountZkEpns; i++) {
                if(it->first == atoi(strings->data[i])){
                    foundEpn = true;
                    break;
                }
            }
            if(!foundEpn){
                LOG(trace) << "found deleted epn: " << it->second;
                break;
            }
        }
        LOG(trace) << "deleting epn: " << it->second;
        listOfAvailableEpns.erase (it);                
        currentChannel = listOfAvailableEpns.end(); //reset
        epnsListChanged = false; //unlock
    }
}

void FirstLineProccessing::epn_watcher (zhandle_t *zh,
                    int type,
                    int state,
                    const char *path,
                    void *watcherCtx)
  {
    LOG(trace) << "epn watcher triggerd" << std::string(path);
    if( type == ZOO_CHILD_EVENT) {
        get_epns();
    }
    LOG(trace) << "epn watcher done";
}
/**
 *
 * Completion function invoked when the call to get
 * the list of epns returns.
 *
 */
void FirstLineProccessing::epn_completion (int rc,
					const struct String_vector *strings,
					const void *data) {
	switch (rc) {
		case ZCONNECTIONLOSS:
		case ZOPERATIONTIMEOUT:
		{
			get_epns();
		}
		break;
		case ZOK:
		{
			LOG(trace) << "!epns list updated! epn nodes:";
            numberOfNewEpns = 0;
            numberOfNewEpnsRetrieved = 0;
			handleZkEpnsUpdate(strings);
			for(int i = 0; i < strings->count; i++) {
                LOG(trace) << strings->data[i];
			}
            LOG(trace) << "amount of new epns: " << to_string(numberOfNewEpns);
		}
		break;
		default:
            LOG(trace) << "Something went wrong when checking epns:"; //to_string(rc)
			break;
	}
}
//asynch retrieev epn and place watcher
void FirstLineProccessing::get_epns () {
		zoo_awget_children(zh,
						"/EPN",
						epn_watcher,
						NULL,
						epn_completion,
						NULL);
}

bool FirstLineProccessing::HandleBroadcast(FairMQParts& msg, int /*index*/)
{
    LOG(trace) << "received heafrtbeat!";
    //listen to heartbeats)
    if(!epnsListChanged && listOfAvailableEpns.size() > 0){
        if(currentChannel == listOfAvailableEpns.end()){
            currentChannel = listOfAvailableEpns.begin();
        }
        
        FairMQMessagePtr msgsend(NewMessage(text.get(),
                                        fTextSize,
                                        [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */ },
                                        text.get()));

        Send(msgsend, to_string(currentChannel->first), 0, 0); // send async
        currentChannel++;
    }
}

std::map<int, std::string> FirstLineProccessing::listOfAvailableEpns;
std::map<int, std::string> FirstLineProccessing::listOfNewEpns;
bool FirstLineProccessing::epnsChanged = false;
bool FirstLineProccessing::epnsListChanged = false;
int FirstLineProccessing::numberOfNewEpns = 0;
int FirstLineProccessing::numberOfNewEpnsRetrieved = 0;

uint64_t FirstLineProccessing::fTextSize = 0;
std::unique_ptr<char[]> FirstLineProccessing::text = nullptr;
const std::string FirstLineProccessing::stateChangeHook = "hook";
std::atomic<bool> FirstLineProccessing::isReconfiguringChannels(false);
std::atomic<bool> FirstLineProccessing::isReinitializing(false);
std::atomic<uint8_t> FirstLineProccessing::currentReconfigureStep(0);

std::map<int, std::string>::iterator FirstLineProccessing::currentChannel = listOfAvailableEpns.begin();

FirstLineProccessing::FirstLineProccessing()
{
    OnData("broadcast", &FirstLineProccessing::HandleBroadcast);
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
        LOG(trace) << " error while initliazing zookeeper";
		//return errno;
	}
	get_epns();
}

void FirstLineProccessing::InitTask()
{
    //if(text != nullptr) delete static_cast<char*>(text);
    fTextSize = fConfig->GetValue<uint64_t>("bytes-per-message");
    LOG(trace) << "Create message of size " << to_string(fTextSize);
    text = unique_ptr<char[]>(new char[fTextSize]);
    for(uint64_t i = 0; i < fTextSize; i++) {
        text[i] = 'a';
    }
}

void FirstLineProccessing::PreRun()
{
    if(isReconfiguringChannels) {
        isReconfiguringChannels = false;
        UnsubscribeFromStateChange(stateChangeHook);
    }
}

bool FirstLineProccessing::ConditionalRun(){
    if(epnsChanged && (numberOfNewEpns == numberOfNewEpnsRetrieved)){
        LOG(trace) << "received all epns information";
        //this gets triggered when 1) the zookeeper watcher of the epn nodes gets triggerd,
        //and 2) when every epn update is retrieveds 

        for (std::map<int, std::string>::iterator it=listOfNewEpns.begin(); it!=listOfNewEpns.end(); ++it){
            //new fairqm channel with name data  and port and ip in value
            FairMQChannel channel("push", "connect", it->second);
            LOG(trace) << "Configure packet:" << it->first << " - " << it->second;
            channel.UpdateRateLogging(1);
            channel.ValidateChannel();
            AddChannel(to_string(it->first), channel);
        }
        listOfNewEpns.clear();
        currentChannel = listOfAvailableEpns.end(); //probably not needed TODO

        // Device re-initialization to configure new channels
        isReinitializing = false;
        currentReconfigureStep = 1;
        SubscribeToStateChange(stateChangeHook, [&](const State curState){
            // Step 1
            if(!isReinitializing) {
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
                    isReinitializing = true;
                    ChangeState("INIT_DEVICE");
                    // WaitForEndOfStateForMs("INIT_DEVICE", 1);
                }
            }
            else {
                LOG(trace) << "Building up";
                // step 4
                if(curState == DEVICE_READY && currentReconfigureStep == 4) {
                    currentReconfigureStep = 5;
                    LOG(trace) << "UP step 4 - Current state is device ready";
                    ChangeState("INIT_TASK");
                    // WaitForEndOfStateForMs("INIT_TASK", 1);
                }
                // step 5
                if(curState == READY && currentReconfigureStep == 5) {
                    currentReconfigureStep = 6;
                    LOG(trace) << "UP step 5 - Current state is ready";
                    ChangeState("RUN");
                }
            }
        });

        // end of reconfigure
        epnsChanged = false;
        epnsListChanged = false;
        return false;
    }
    return true;
}


FirstLineProccessing::~FirstLineProccessing()
{
    //delete static_cast<char*>(text);
}
