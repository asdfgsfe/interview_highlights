#ifndef FUTURE_H
#define FUTURE_H

#include <mutex>
#include <condition_variable>

// 可以使用std::future和std::promise来实现

template <typename T>
class Result
{
public:
    virtual T getResultValue() = 0;
    virtual ~Result() {}
};

template <typename T>
class RealResult : public Result<T>
{
public:
    explicit RealResult(const T &resultValue)
        : resultValue_(resultValue)
    {}

    T getResultValue() override
    {
        return resultValue_;
    }

private:
    T resultValue_;
};

template <typename T>
class FutureResult : public Result<T>
{
public:
    FutureResult()
        : result_(nullptr),
          readyFlag_(false)
    {

    }

    ~FutureResult()
    {
        delete result_;
    }

    void setResult(Result<T> *result)
    {
        std::unique_lock<std::mutex> guard(mutex_);
        readyFlag_ = true;
        if (result_) {
            delete result_;
            result_ = nullptr;
        }
        result_ = result;
        ready_.notify_all();
    }

    T getResultValue() override
    {
        std::unique_lock<std::mutex> guard(mutex_);
        ready_.wait(guard, [this] {
            return readyFlag_;
        });
        return result_;
    }

private:
    Result<T> *result_;
    bool readyFlag_;
    std::mutex mutex_;
    std::condition_variable ready_;
};

#endif // FUTURE_H
