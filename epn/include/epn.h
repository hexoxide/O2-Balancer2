#ifndef EPN_H
#define EPN_H

#include "FairMQDevice.h"

class EventProccessingNode : public FairMQDevice
{
  public:
    EventProccessingNode();
    virtual ~EventProccessingNode();

  protected:
  	virtual void InitTask();

	bool HandleData(FairMQMessagePtr&, int);

  	uint64_t fNumFlp;
    std::string address;
  	
  	uint64_t receivedMessages;

  private:
};

#endif /* EPN_H */
