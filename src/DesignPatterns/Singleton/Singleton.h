#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>
#include <atomic>
#include <memory>
#include <mutex>
#include <stdlib.h>

class Singleton
{
public:
    static Singleton& getInstance();

private:
    Singleton() {}
    ~Singleton() {}

    static void destroy() {
        delete pInstance_;
    }

    Singleton(const Singleton&) = delete;
    Singleton &operator=(const Singleton&) = delete;

    static std::atomic<Singleton*> pInstance_;
    static std::mutex mutex_;
};

std::atomic<Singleton*> Singleton::pInstance_ = ATOMIC_VAR_INIT(nullptr);
std::mutex Singleton::mutex_;

Singleton& Singleton::getInstance()
{
    Singleton* tmp = pInstance_.load(std::memory_order_acquire); // acquire-release
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        tmp = pInstance_.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new Singleton();
            ::atexit(destroy);
            pInstance_.store(tmp, std::memory_order_release);
        }
    }
    return *pInstance_;
}

#endif // SINGLETON_H
