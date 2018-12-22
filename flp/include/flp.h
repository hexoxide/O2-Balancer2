#ifndef FLP_H
#define FLP_H

#include <thread>
#include <chrono>
#include <string>

#include "FairMQDevice.h"
#include "zookeeper/zookeeper.h"

class FirstLineProccessing : public FairMQDevice
{
  public:
    FirstLineProccessing();
    virtual ~FirstLineProccessing();

  protected:
    void InitTask() override;
    void PreRun() override;

    //bool HandleBroadcast(FairMQParts&, int);
    static void get_task_data(const char *);
    static void get_task_data_completion(int rc, const char *value, int value_len, const struct Stat *stat, const void *data);
    static void assign_tasks(const struct String_vector *strings);
    static void get_epns();
    static void epn_watcher (zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
    static void epn_completion (int rc, const struct String_vector *strings, const void *data);

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
