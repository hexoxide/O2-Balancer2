#ifndef ICN_H
#define ICN_H

// Standard library
#include <thread>
#include <chrono>
#include <string>

// Dependencies
#include "FairMQDevice.h"

// Local
#include "o2channel.h"

class InformationControlNode : public FairMQDevice
{
  public:
    InformationControlNode();
    virtual ~InformationControlNode();

  protected:
    void InitTask() override;
    void PreRun() override;
    bool ConditionalRun() override;
    void PostRun() override;

    void ListenForFeedback();
    void ExitDevice(const State);

    uint64_t fIterations;

    // used to exit device
    const std::string stateChangeHook;
    std::atomic<uint8_t> currentReconfigureStep;

    uint64_t numHeartbeat;
    std::thread feedbackListener;
    std::vector<O2Channel> channels;
    bool isConfigure;
    std::chrono::system_clock::time_point startTime;
};

#endif /* ICN_H */