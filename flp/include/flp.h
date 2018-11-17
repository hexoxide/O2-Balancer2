#ifndef FLP_H
#define FLP_H

#include "FairMQDevice.h"

class FirstLineProccessor : public FairMQDevice
{
  public:
    FirstLineProccessor();
    virtual ~FirstLineProccessor();

  protected:
    virtual void InitTask();
    bool HandleData(FairMQMessagePtr&, int);

  private:
    uint64_t fMaxIterations;
    uint64_t fNumIterations;
};

#endif /* FLP_H */