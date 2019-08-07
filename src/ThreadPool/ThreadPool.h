#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "Noncopyable.h"
#include <assert.h>
#include <deque>
#include <mutex>
#include <thread>
#include <functional>
#include <vector>
#include <condition_variable>

// A simple version of thread pool.
class ThreadPool : zen::noncopyable
{
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(int numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void start();
    void stop();

    void setQueueSize(size_t queuesize) {
        taskQueueSize_ = queuesize;
    }
    void put(const Task& f);

private:
    void runInThread();
    Task take();

    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;

    std::vector<std::thread> threads_;

    std::deque<Task> taskQueue_;
    bool running_;
    size_t numThreads_;
    size_t taskQueueSize_;
};

#endif // THREADPOOL_H
