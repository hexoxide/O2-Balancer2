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
  	char* text;
    
    uint64_t fTextSize;
    std::string fCurrentChannel;

    bool HandleBroadcast(FairMQMessagePtr&, int);

    void InitTask() override;
    // bool ConditionalRun() override;
};

#endif /* FLP_H */