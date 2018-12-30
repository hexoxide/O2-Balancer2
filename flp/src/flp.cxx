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

void FirstLineProccessing::get_task_data_completion(int rc, const char *value, int value_len,
                              const struct Stat *stat, const void *data) {
    //int worker_index;

    switch (rc) {
        case ZCONNECTIONLOSS:
        case ZOPERATIONTIMEOUT:
            get_task_data((const char *) data);

            break;

        case ZOK:
        {
            char* nodeName = (char *) data;
            char* nodeValue = strndup(value, value_len); 
            isReconfiguringChannels = true;
            LOG(trace) << "Configuring";

            int epnID = std::stoi (nodeName, nullptr);
            listOfEpns[epnID] = nodeValue;

            numberOfNewEpnsRetrieved += 1;
            // printf("key: \t");
            // printf("%s\n", nodeName);
            // printf("value \t");
            // printf("%s\n", nodeValue);
        }
            break;
        // default:
        //     //LOG_ERROR(("Something went wrong when checking the master lock: %s",rc2string(rc)));
        //     break;
    }
}
void FirstLineProccessing::get_task_data(const char *task) {
    if(task == NULL) return;
    char * tmp_task = strndup(task, 15);
    char * path = make_path(2, "/EPN/", tmp_task);
    //LOG_DEBUG(("Getting task data %s",tmp_task));
    //data_completion_t get_task_data_completion_bound = <decltype( std::bind(&FirstLineProccessing::get_task_data_completion, this, _1, _2, _3, _4, _5) )>();
    
    zoo_aget(zh,
             path,
             0,
             get_task_data_completion,
             (const void *) tmp_task);
    free(path);
    free(tmp_task);
}
void FirstLineProccessing::assign_tasks(const struct String_vector *strings) {
    /*
     * for each epn receive ip and port
     */
    //LOG_DEBUG(("Task count: %d", strings->count));
    int i;
    for( i = 0; i < strings->count; i++) {
        //LOG_DEBUG(("Assigning task %s", char *) strings->data[i]));
        std::map<int, std::string>::iterator iterator = listOfEpns.find(atoi(strings->data[i]));
        if (iterator != listOfEpns.end()){
            //this does not yet check on nodes that went offline
            numberOfNewEpns += 1;
            get_task_data( strings->data[i] );
        }
    }
}

void FirstLineProccessing::epn_watcher (zhandle_t *zh,
                    int type,
                    int state,
                    const char *path,
                    void *watcherCtx)
  {
    printf("epn watcher triggered %s\n", std::string(path).c_str());
    if( type == ZOO_CHILD_EVENT) {
        get_epns();
    }
    printf("Tasks watcher done\n");
}
/**
 *
 * Completion function invoked when the call to get
 * the list of tasks returns.
 *
 */
void FirstLineProccessing::epn_completion (int rc,
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
			printf("!epns list updated!\n");
            epnsChanged = true;
            numberOfNewEpns = 0;
            numberOfNewEpnsRetrieved = 0;
			// struct String_vector *tmp_tasks = added_and_set(strings, &epns);
			assign_tasks(strings);
			for(int i = 0; i < strings->count; i++) {
				printf("%s", strings->data[i]);
			}
			//free_vector(tmp_tasks);
		}
		break;
		default:
			printf("Something went wrong when checking epns: %d", rc);

			break;
	}
}
//asynch retrieev epn and place watcher
void FirstLineProccessing::get_epns () {
	printf("Getting tasks\n");
		zoo_awget_children(zh,
						"/EPN",
						epn_watcher,
						NULL,
						epn_completion,
						NULL);
}

std::map<int, std::string> FirstLineProccessing::listOfEpns;
bool FirstLineProccessing::epnsChanged = false;
int FirstLineProccessing::numberOfNewEpns = 0;
int FirstLineProccessing::numberOfNewEpnsRetrieved = 0;

uint64_t FirstLineProccessing::fTextSize = 0;
std::unique_ptr<char[]> FirstLineProccessing::text = nullptr;
const std::string FirstLineProccessing::stateChangeHook = "hook";
std::atomic<bool> FirstLineProccessing::isReconfiguringChannels = false;
std::atomic<bool> FirstLineProccessing::isReinitializing = false;
std::atomic<uint8_t> FirstLineProccessing::currentReconfigureStep = 0;

std::string FirstLineProccessing::currentChannel = 1;

FirstLineProccessing::FirstLineProccessing()
{
    //OnData("broadcast", &FirstLineProccessing::HandleBroadcast);
    char buffer[512];
	printf("starting program\n");
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
	if (!zh) {
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
        //this gets triggered when 1) the zookeeper watcher of the epn nodes gets triggerd,
        //and 2) when every epn update is retrieveds 

        //first delete all channels
        //then create new channels
        for (std::map<int, std::string>::iterator it=listOfEpns.begin(); it!=listOfEpns.end(); ++it){
            //new fairqm channel with name data  and port and ip in value
            FairMQChannel channel("push", "connect", it->second);
            LOG(trace) << "Configure packet:" << it->first << " - " << it->second;
            channel.UpdateRateLogging(1);
            channel.ValidateChannel();
            AddChannel(to_string(it->first), channel);
        }
        
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
        //this will have to be implemented in the run method (we still have to listen to heartbeats)

        // FairMQMessagePtr msgsend(NewMessage(text.get(),
        //                                 fTextSize,
        //                                 [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */ },
        //                                 text.get()));

        // Send(msgsend, currentChannel, 0, 0); // send async
        epnsChanged = false;
    }
    return true;
}


FirstLineProccessing::~FirstLineProccessing()
{
    //delete static_cast<char*>(text);
}
