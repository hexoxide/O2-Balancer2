#ifndef EPN_H
#define EPN_H

#include "FairMQDevice.h"

class EventProccessingNode : public FairMQDevice
{
  public:
    EventProccessingNode();
    virtual ~EventProccessingNode();

  protected:
    static const std::string feedbackChannel;
    static const std::string pNumberOfFLP;

  	virtual void InitTask();

    bool HandleData(FairMQParts& msg, int);

  	uint64_t fNumFlp;
  	
  	uint64_t receivedMessages;
    uint64_t currentHeartbeat;
    bool     isCurrentlyOutOfOrder;

  private:
};

#endif /* EPN_H */