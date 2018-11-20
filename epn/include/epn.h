#ifndef EPN_H
#define EPN_H

#include "FairMQDevice.h"

class EventProccessingNode : public FairMQDevice
{
  public:
    EventProccessingNode();
    virtual ~EventProccessingNode();

  protected:
  	uint64_t fNumFlp;
  	uint64_t receivedMessages;

    virtual void InitTask();

    bool HandleData(FairMQMessagePtr&, int);

  private:
};

#endif /* EPN_H */