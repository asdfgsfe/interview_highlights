#ifndef ACTIVATIONQUEUE_H
#define ACTIVATIONQUEUE_H

#include "MethodRequest.h"
#include <stdint.h>
#include <mutex>
#include <condition_variable>
#include <queue>

class ActivationQueue
{
public:
    const int INFINITE = -1;

    ActivationQueue(size_t highWaterMark) :capacity_(highWaterMark) { }

    void enqueue (MethodRequest *mr, long timeout = INFINITE)
    {
        std::unique_lock<std::mutex> guard(mutex_);
        notFull_.wait(guard, [this] {
            return queue_.size() < capacity_;
        });
        queue_.push(mr);
        notEmpty_.notify_one();
    }

    MethodRequest* dequeue (long timeout = INFINITE)
    {
        std::unique_lock<std::mutex> guard(mutex_);
        notEmpty_.wait(guard, [this] {
            return !queue_.empty();
        });
        MethodRequest *mr = queue_.front();
        queue_.pop();
        return mr;
    }

private:
    std::queue<MethodRequest> queue_;
    int capacity_;
    std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

#endif // ACTIVATIONQUEUE_H
