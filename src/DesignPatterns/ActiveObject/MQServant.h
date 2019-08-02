#ifndef MQSERVANT_H
#define MQSERVANT_H

#include <stdint.h>
#include "Future.h"
#include <string>

class ActiveObject
{
public:
    virtual ~ActiveObject() { }
    virtual Result<std::string>* add (const std::string &x, const std::string &y) = 0;
};

// 请求的执行者
class MQServant : public ActiveObject
{
public:
    MQServant() = default;
    ~MQServant() = default;

    Result<std::string>* add (const std::string &x, const std::string &y) override
    {
        return new RealResult<std::string>(std::to_string(std::stoi(x) + std::stoi(y)));
    }
};

#endif // MQSERVANT_H
