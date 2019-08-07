#ifndef THREADPOOL_V2_H
#define THREADPOOL_V2_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>
#include "Noncopyable.h"

/// 源于百度Apollo
/// future and promise model.

class ThreadPool_V2 : zen::noncopyable
{
public:
    typedef std::function<void()> Task;

    explicit ThreadPool_V2(size_t);
    ~ThreadPool_V2();

    // C++14语法可以简化尾序返回类型
    template <typename Callable, typename... Args>
    auto enqueue(Callable&& f, Args&&... args)
    -> std::future<typename std::result_of<Callable(Args...)>::type>;

    void Stop();

private:
    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex queue_mutex;
    std::condition_variable cond_;
    bool stop_;
};

// the constructor just launches some amount of workers
inline ThreadPool_V2::ThreadPool_V2(size_t threads)
    : stop_(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers_.emplace_back([this] {
            for (;;) {
                Task task;

                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    cond_.wait(lock, [this] {
                        return stop_ || !this->tasks_.empty();
                    });
                    if (stop_ && tasks_.empty()) return;
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
        });
}

template <class Callable, class... Args>
auto ThreadPool_V2::enqueue(Callable&& f, Args&&... args)
    -> std::future<typename std::result_of<Callable(Args...)>::type>
{
    using return_type = typename std::result_of<Callable(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()> >(
                std::bind(std::forward<Callable>(f), std::forward<Args>(args)...));

    // A packaged_task wraps a callable element and allows its result to be retrieved asynchronously.
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks_.emplace([task]() { (*task)(); });
    }
    cond_.notify_one();
    return res;
}

void ThreadPool_V2::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop_ = true;
    }
    cond_.notify_all();

    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

ThreadPool_V2::~ThreadPool_V2()
{
    if (!stop_) {
        Stop();
    }
}

#endif // THREADPOOL_H
