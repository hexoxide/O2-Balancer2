#ifndef FLP_H
#define FLP_H

#include <thread>
#include <chrono>
#include <string>

#include "FairMQDevice.h"

class FirstLineProccessing : public FairMQDevice
{
  public:
    FirstLineProccessing();
    virtual ~FirstLineProccessing();

  protected:
    void InitTask() override;
    void PreRun() override;
    //bool ConditionalRun() override;

    bool HandleBroadcast(FairMQParts&, int);

    uint64_t fTextSize;

    // TODO AliceO2 coding guidelines unique pointer
  	char* text;
    std::atomic<bool> isReconfiguringChannels;
    std::atomic<bool> isReinitializing;
    std::atomic<uint8_t> currentReconfigureStep;
    uint64_t lastHeartbeat;
    std::string currentChannel;
};

#endif /* FLP_H */