#include "flp.h"

using namespace std;

FirstLineProccessing::FirstLineProccessing()
    : fTextSize(0)
    , currentChannel("1")
{
    OnData("broadcast", &FirstLineProccessing::HandleBroadcast);
}

bool FirstLineProccessing::HandleBroadcast(FairMQMessagePtr& msg, int /*index*/)
{
    // LOG(info) << "Received broadcast: \"" << string(static_cast<char*>(msg->GetData()), msg->GetSize()) << "\"";
    currentChannel = string(static_cast<char*>(msg->GetData()),msg->GetSize());

    FairMQMessagePtr msgsend(NewMessage(text,
                                    fTextSize,
                                    [](void* /*data*/, void* object) { /*delete static_cast<char*>(object); */},
                                    text));

    LOG(info) << currentChannel << "\"";
    if (Send(msgsend, currentChannel) < 0)
    {
        return false;
    }
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