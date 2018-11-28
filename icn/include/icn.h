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

    std::thread fFeedbackListener;
    uint64_t fNumHeartbeat;

    std::vector<O2Channel> channels;
    bool isConfigure;
};

#endif /* ICN_H */