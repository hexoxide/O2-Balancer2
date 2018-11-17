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
  	char* text;
    uint64_t fTextSize;
    uint64_t fMaxIterations;
    uint64_t fNumIterations;

    void InitTask() override;
    bool ConditionalRun() override;
};

#endif /* ICN_H */