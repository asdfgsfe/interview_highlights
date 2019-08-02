#ifndef METHODREQUEST_H
#define METHODREQUEST_H

#include "Future.h"
#include "MQServant.h"
#include <string>

// Command pattern
// Interface of the operations
template <typename T>
class MethodRequest
{
protected:
    MethodRequest(FutureResult<T> *future, MQServant *servant)
        : result_(future),
          servant_(servant)
    {
    }

public:
    // Evaluate the synchronization constraint.
    virtual bool guard(void)
    {
        return true;
    }

    // Implement the method
    virtual void execute(void) = 0;

private:
    FutureResult<T> *result_;
    MQServant *servant_;
};

// 对应Proxy的add()方法调用
class AddRequest : public MethodRequest<std::string>
{
public:
    AddRequest(MQServant *servant,
               FutureResult<T> *future,
               const std::string &arg1,
               const std::string &arg2)
        : MethodRequest<std::string>(future, servant),
          arg1_(arg1),
          arg2_(arg2)
    {}

    void execute(void) override
    {
        result_->setResult(servant_->add(arg1_, arg2_););
    }

private:
    std::string arg1_;
    std::string arg2_;
};

#endif // METHODREQUEST_H
