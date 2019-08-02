#ifndef MQSCHEDULER_H
#define MQSCHEDULER_H

#include "ActivationQueue.h"
#include <thread>

// 调度器运行在一个线程中
class MQScheduler
{
public:
    MQScheduler(size_t highWaterMark)
        : activeq_ (new ActivationQueue(highWaterMark)),
          thr(new std::thread(&MQScheduler::dispatch, this))
    {
    }

    ~MQScheduler()
    {
        if (thr) {
            if (thr->joinable()) {
                thr->join();
            }
            delete thr;
        }
    }
    void invoke (MethodRequest *mr)
    {
        if (activeq_) {
            activeq_->enqueue(mr);
        }
    }

    // Dispatch the Method Requests on their Servant in the Scheduler’s thread.
    void dispatch ()
    {
        if (!activeq_) return;
        // Iterate continuously in a separate thread.
        for ( ; ; ) {
            for (ActivationQueue::iterator i = activeq_->begin (); i != activeq_->end (); i++) {
                MethodRequest *mr = *i;
                if (mr->guard ()) {
                    // Remove <mr> from the queue first in case <call> throws an exception.
                    activeq_->dequeue(mr);
                    mr->execute ();
                    delete mr;
                }
            }
        }
    }

protected:
    ActivationQueue *activeq_;
    std::thread *thr;
};

#endif // MQSCHEDULER_H
