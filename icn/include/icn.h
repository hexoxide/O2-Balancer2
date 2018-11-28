#ifndef ICN_H
#define ICN_H

#include <thread>
#include <chrono>
#include <string>

#include "FairMQDevice.h"

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
    bool isConfigure;
};

#endif /* ICN_H */