#ifndef ICN_H
#define ICN_H

// Standard library
#include <thread>
#include <chrono>
#include <string>

// Dependencies
#include "FairMQDevice.h"

// Local
#include "data/o2channel.h"

/// Supervise the other nodes.
/** 
 * The Information Control Node leads the experiment by 
 * determining how long it should run and what the pace is. It 
 * sends requests to the other nodes with this information.
 */
class InformationControlNode : public FairMQDevice
{
  public:
    InformationControlNode();
    ~InformationControlNode() override;

    uint64_t getNumberOfChannels();

    uint16_t initialDelay; /**< A pre-set amount of time needed to configure the node on startup. */

  protected:
    void InitTask() override;
    void PreRun() override;
    bool ConditionalRun() override;
    void PostRun() override;

    uint64_t fIterations; /**< How many times the node should broadcast. */

    uint64_t determineChannel();

    void ListenForFeedback();
    std::thread feedbackListener;
    // bool HandleFeedback(FairMQParts&, int);

    uint64_t numHeartbeat; /**< Keeps track of the iterations. */
    uint64_t numAcknowledge; /**< How many times the node received an acknowledge. */
    
    // TODO std::unique_ptr
    std::vector<O2::data::O2Channel*> channels; /**< The connected channels to broadcast to. */
    O2::data::O2Channel* currentChannel;
    std::atomic<bool> isConfigure;
    std::atomic<bool> isPreConfigure;
    std::chrono::system_clock::time_point startTime;
};

#endif /* ICN_H */