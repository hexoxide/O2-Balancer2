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
    ~InformationControlNode() override;

  protected:
    void InitTask() override;
    void PreRun() override;
    bool ConditionalRun() override;
    void PostRun() override;

    uint64_t fIterations;

    void ListenForFeedback();
    std::thread feedbackListener;
    // bool HandleFeedback(FairMQParts&, int);

    uint64_t numHeartbeat;
    uint64_t numAcknowledge;
    // TODO std::unique_ptr
    std::vector<O2Channel*> channels;
    std::atomic<bool> isConfigure;
    std::atomic<bool> isPreConfigure;
    std::chrono::system_clock::time_point startTime;
};

#endif /* ICN_H */