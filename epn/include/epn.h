#ifndef EPN_H
#define EPN_H

#include "FairMQDevice.h"

/**
 * Receives messages from the FLP and determines if an ackownledgements should be send to the ICN.
 */
class EventProcessingNode : public FairMQDevice
{
  public:
    EventProcessingNode();
    ~EventProcessingNode() override;

    uint16_t initialDelay;
    
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