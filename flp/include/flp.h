#ifndef FLP_H
#define FLP_H

#include <thread>
#include <chrono>
#include <string>

#include "FairMQDevice.h"

/// Sends data to EPN's.
/**
 * The First Line Processor is responsible for sending data
 * to the Event Processing Node.
 */
class FirstLineProcessor : public FairMQDevice
{
  public:
    FirstLineProcessor();
    ~FirstLineProcessor() override;

  protected:
    void InitTask() override;
    void PreRun() override;

    bool HandleBroadcast(FairMQParts&, int);

    void reinitialize();

    uint64_t fTextSize;

  	std::unique_ptr<char[]> text;
    
    // used to reinitialize channels
    const std::string stateChangeHook;
    std::atomic<bool> isReconfiguringChannels;
    std::atomic<bool> isReinitializing;
    std::atomic<uint8_t> currentReconfigureStep;

    uint64_t lastHeartbeat;
    std::string currentChannel;
};

#endif /* FLP_H */