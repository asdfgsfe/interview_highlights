#include <iostream>
#include <assert.h>
#include "MQServant.h"
#include "Future.h"
#include <thread>

// 参考面向模式的软件架构：卷二 P258
// TODO: 使用std::promise/std::packaged_task, std::future等实现

class ClientThread
{
public:
   explicit ClientThread(ActiveObject *activeObj)
        :activeObject(activeObj)
    {
        assert(activeObject != nullptr);
        thr = new std::thread(&ClientThread::threadFun, this);
    }

    ~ClientThread()
    {
        if (thr) {
            if (thr->joinable()) {
                thr->join();
            }
            delete thr;
        }
    }

private:
    void threadFun()
    {
        assert(activeObject != nullptr);
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            Result<std::string>* result = activeObject->add("1", "2");
            std::cout << "result: " << result->getResultValue() << std::endl;
            delete result;
        }
    }

    ActiveObject* activeObject;
    std::thread *thr;
};

int main()
{
    MQServant *servant = new MQServant;
    ClientThread client(servant);

    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    delete servant;
    return 0;
}
