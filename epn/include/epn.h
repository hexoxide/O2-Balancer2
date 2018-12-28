#ifndef EPN_H
#define EPN_H

#include "FairMQDevice.h"

class EventProccessingNode : public FairMQDevice
{
  public:
    EventProccessingNode();
    ~EventProccessingNode() override;
    
    static const std::string pNumberOfFLP;
    static const std::string pPrimaryNetworkInterface;

  protected:
    static const std::string feedbackChannel;

  	void InitTask() override;
    void PreRun() override;

    bool HandleData(FairMQParts& msg, int);

  	uint64_t      fNumFlp;
    std::string   fPrimaryNetworkInterface;
  	
  	uint64_t receivedMessages;
    uint64_t currentHeartbeat;
    bool     isCurrentlyOutOfOrder;

  private:
};

#endif /* EPN_H */