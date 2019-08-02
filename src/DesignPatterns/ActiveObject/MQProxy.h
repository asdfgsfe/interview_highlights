#ifndef MQPROXY_H
#define MQPROXY_H

#include "Future.h"
#include "MethodRequest.h"
#include "MQScheduler.h"
#include "MQServant.h"

// 客户端使用这个类
class MQProxy : public ActiveObject
{
public:
    // Bound the message queue size.
    enum { MAX_SIZE = 100 };

    MQProxy (size_t size = MAX_SIZE)
        : scheduler_ (new MQScheduler(size)),
          servant_ (new MQServant())
    { }

    ~MQProxy() {
        delete scheduler_;
        delete servant_;
    }

    // Schedule <add> to execute on the active object.
    Result<std::string>* add (const std::string &x, const std::string &y) {
        Result<std::string>* future = new FutureResult<std::string>();
        scheduler_->invoke(new AddRequest(servant_, future, x, y));
        return future;
    }

protected:
    // The Servant that implements the Active Object methods.
    MQServant *servant_;
    // A scheduler for the Message Queue.
    MQScheduler *scheduler_;
};

#endif // MQPROXY_H
