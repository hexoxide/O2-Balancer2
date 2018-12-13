#include "flp.h"

#include "o2data.h"
#include "o2channel.h"

using namespace std;

FirstLineProccessing::FirstLineProccessing()
    : fTextSize(0)
    , text(nullptr)
    , stateChangeHook("hook")
    , isReconfiguringChannels(false)
    , isReinitializing(false)
    , currentReconfigureStep(0)
    , lastHeartbeat(0)
    , currentChannel("1")
{
    OnData("broadcast", &FirstLineProccessing::HandleBroadcast);
}

bool FirstLineProccessing::HandleBroadcast(FairMQParts& msg, int /*index*/)
{
    bool isFirstMessagePart = true;
    O2Data* data;

    // Iterate through all message parts
    for (const auto& part : msg) {
        // First part should always be of O2Data 
        if(isFirstMessagePart) 
        {
            isFirstMessagePart = false;
            data = static_cast<O2Data*>(part->GetData());
            currentChannel = to_string(data->tarChannel);
            if(data->configure) 
            {
                isReconfiguringChannels = true;
                LOG(trace) << "Configuring";
            }
            continue;
        }
        
        // Add channels to device based on O2Channel message part data
        if(isReconfiguringChannels)
        {
            O2Channel* data2 = static_cast<O2Channel*>(part->GetData());
            string name = to_string(data2->index);
            string ip = to_string(data2->ip1) + "." + to_string(data2->ip2) + "." + to_string(data2->ip3) + "." + to_string(data2->ip4);
            string port = to_string(data2->port);
            FairMQChannel channel("push", "connect", "tcp://" + ip + ":" + to_string(data2->port));
            LOG(trace) << "Configure packet:" << name << " - " << ip << ":" << port;
            channel.UpdateRateLogging(1);
            channel.ValidateChannel();
            AddChannel(name, channel);
        }
    }

    // Device re-initialization to configure new channels
    if(isReconfiguringChannels) {
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
        return false;
    }
    // end of reconfigure

    FairMQMessagePtr msgsend(NewMessage(text.get(),
                                    fTextSize,
                                    [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */ },
                                    text.get()));

    Send(msgsend, currentChannel, 0, 0); // send async
    return true;
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

FirstLineProccessing::~FirstLineProccessing()
{
    //delete static_cast<char*>(text);
}