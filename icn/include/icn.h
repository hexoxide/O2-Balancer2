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
    bool ConditionalRun() override;
    void PostRun() override;

    FairMQPollerPtr poller;
};

#endif /* ICN_H */