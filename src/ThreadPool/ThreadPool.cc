#include "ThreadPool.h"

#include <stdio.h>
#include <algorithm>

ThreadPool::ThreadPool(int numThreads)
    : running_(false),
      numThreads_(numThreads),
      taskQueueSize_(numThreads_ * 10)
{

}

ThreadPool::~ThreadPool()
{
    if (running_) {
        stop();
    }
}

void ThreadPool::start()
{
    threads_.reserve(numThreads_);
    running_ = true;
    for (unsigned int i = 0; i < numThreads_; ++i) {
        threads_.emplace_back(&ThreadPool::runInThread, this);
    }
}

void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        notEmpty_.notify_all();
    }

    std::for_each(threads_.begin(), threads_.end(),
                  std::bind(&std::thread::join, std::placeholders::_1));
}

void ThreadPool::put(const Task &task)
{
    if (threads_.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this]() {
            return taskQueue_.size() < taskQueueSize_;
        });

        taskQueue_.push_back(task);
        notEmpty_.notify_one();
    }
}

ThreadPool::Task ThreadPool::take()
{
    std::unique_lock<std::mutex> lock(mutex_);
    notEmpty_.wait(lock, [this]() {
        return !taskQueue_.empty() || !running_;
    });

    Task task;
    if (!taskQueue_.empty()) {
        task = taskQueue_.front();
        taskQueue_.pop_front();

        notFull_.notify_one();
    }

    return task;
}

void ThreadPool::runInThread()
{
    while (running_)
    {
        Task task(take());
        if (task)
        {
            task();
        }
    }
}
