#include "flp.h"

#include "o2data.h"
#include "o2channel.h"

using namespace std;

FirstLineProccessing::FirstLineProccessing()
    : fTextSize(0)
    , lastHeartbeat(0)
    , currentChannel("1")
{
    OnData("broadcast", &FirstLineProccessing::HandleBroadcast);
}

bool FirstLineProccessing::HandleBroadcast(FairMQParts& msg, int /*index*/)
{
    bool isFirst = true;
    bool isConfiguring = false;
    O2Data* data;

    for (const auto& part : msg) {
        if(isFirst) 
        {
            isFirst = false;
            data = static_cast<O2Data*>(part->GetData());
            LOG(ERROR) << "First";
            if(data->configure) 
            {
                isConfiguring = true;
                LOG(ERROR) << "Configuring";
            }
            continue;
        }
        
        if(isConfiguring)
        {
            O2Channel* data2 = static_cast<O2Channel*>(part->GetData());
            string name = to_string(data2->index);
            string ip = to_string(data2->ip1) + "." + to_string(data2->ip2) + "." + to_string(data2->ip3) + "." + to_string(data2->ip4);
            string port = to_string(data2->port);
            FairMQChannel channel("push", "connect", "tcp://" + ip + ":" + to_string(data2->port));
            LOG(ERROR) << "Configure packet:" << name << " - " << ip << ":" << port;
            channel.UpdateRateLogging(1);
            channel.ValidateChannel();
            AddChannel(name, channel);
        }
    }
    if(isConfiguring) {
        bool reInit = false;
        const std::string hook("hook");
        SubscribeToStateChange(hook, [&](const State curState){
            // Step 1
            if(!reInit) {
                if(curState == READY) 
                {
                    LOG(ERROR) << "DOWN step 1 - Current state is ready";
                    ChangeState("RESET_TASK");
                }
                // Step 2
                if(curState == DEVICE_READY)
                {
                    LOG(ERROR) << "DOWN step 2 - Current state is device ready";
                    ChangeState("RESET_DEVICE");
                }
                // Step 3
                if(curState == IDLE)
                {
                    LOG(ERROR) << "DOWN step 3 - Current state is idle";
                    reInit = true;
                    ChangeState("INIT_DEVICE");
                }
            }
            else {
                // step 4
                if(curState == DEVICE_READY) {
                    LOG(ERROR) << "UP step 4 - Current state is device ready";
                    reInit = true;
                    ChangeState("INIT_TASK");
                }
                // step 5
                if(curState == READY) {
                    LOG(ERROR) << "UP step 5 - Current state is ready";
                    ChangeState("RUN");
                }
            }
        });
        return false;
    }
    FairMQMessagePtr msgsend(NewMessage(text,
                                    fTextSize,
                                    [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */ },
                                    text));

    Send(msgsend, currentChannel, 0, 0); // send async
    return true;
}

void FirstLineProccessing::InitTask()
{
    // Get the fText and fMaxIterations values from the command line options (via fConfig)
    fTextSize = fConfig->GetValue<uint64_t>("bytes-per-message");
    text = new char[fTextSize];
    for(uint64_t i = 0; i < fTextSize; i++) {
        text[i] = 'a';
    }
    UnsubscribeFromStateChange("hook");
}

void FirstLineProccessing::PostRun()
{
    // ChangeState("RESET_TASK");
}

// bool FirstLineProccessing::ConditionalRun()
// {
//     // create message object with a pointer to the data buffer,
//     // its size,
//     // custom deletion function (called when transfer is done),
//     // and pointer to the object managing the data buffer
//     FairMQMessagePtr msg(NewMessage(text,
//                                     fTextSize,
//                                     [](void* /*data*/, void* object) { delete static_cast<char*>(object); },
//                                     text));

//     // LOG(info) << "Sending \"" << text << "\"";

//     // in case of error or transfer interruption, return false to go to IDLE state
//     // successfull transfer will return number of bytes transfered (can be 0 if sending an empty message).
//     if (Send(msg, currentChannel) < 0)
//     {
//         return false;
//     }

//     return true;
// }

FirstLineProccessing::~FirstLineProccessing()
{
    delete static_cast<char*>(text);
}