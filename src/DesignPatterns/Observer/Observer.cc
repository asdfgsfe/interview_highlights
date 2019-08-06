#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

/// Thread-safe observer pattern
/// 观察者模式定义对象间一对多的依赖关系，当一个对象的状态发生变化时，所有依赖于它的对象都得到
/// 通知并自动更新.

/// 观察者模式的典型应用：将应用的界面与底层应用数据分离（将同一组数据以不同的形式显示出来）

/// 多线程编程中在构造函数中不要泄露this指针.
///
/// (1) 观察者对象和主题对象会被不同的线程操纵
/// (2) 一个观察者仅观察一个主题，一个主题可被多个观察者观察
/// (3) 用shared_ptr和weak_ptr来管理内存，可实现安全的销毁
/// (4) 很明显，观察者模式满足依赖倒置原则（或好莱坞原则）

class Observeralbe;

// 观察者的抽象基类
class Observer
{
public:
    virtual ~Observer() { }
    virtual void update(const std::shared_ptr<Observeralbe>& subject) = 0;
};

// 主题的基类（混入类）
class Observeralbe : public std::enable_shared_from_this<Observeralbe>
{
protected:
    Observeralbe() = default;

public:
    virtual ~Observeralbe() {}
    void registerObserver(const std::weak_ptr<Observer> &o)
    {
        observers_.push_back(o);
    }

    // don't need unregister manually

    void notifyAll()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        auto it = observers_.begin();
        while (it != observers_.end()) {
            std::shared_ptr<Observer> obj(it->lock());
            if (obj) {
                obj->update(this->shared_from_this());
                ++it;
            } else {
                observers_.erase(it);
            }
        }
    }

private:
    std::vector<std::weak_ptr<Observer>> observers_;
    std::mutex mutex_;
};

//------------------------------------------------------------------------------
// 主题的具体类：发布天气情况
class WeatherData : public Observeralbe
{
public:
    WeatherData() = default;
    ~ WeatherData() = default;

    struct Measurement
    {
        float temperature_;
        float humidity_;
        float pressure_;
    };

    void setMeasurements(const Measurement& measurement)
    {
        measurement_ = measurement;
        notifyAll();
    }

    // return by value
    Measurement getMeasurements()
    {
        return measurement_;
    }

private:
    Measurement measurement_;
};

// 观察者具体类：显示天气情况
class ConditionDisplayer : public Observer
{
public:
    explicit ConditionDisplayer(const std::shared_ptr<WeatherData> &subject)
        : subject_(subject)
    { }
    ~ConditionDisplayer() = default;

    //void update(float temp, float humidity, float pressure) override;
    void update(const std::shared_ptr<Observeralbe>& subject) override;
    void display();

private:
    WeatherData::Measurement measurement_;
    const std::shared_ptr<WeatherData> subject_;
};

void ConditionDisplayer::update(const std::shared_ptr<Observeralbe>& subject)
{
    if (subject == subject_ && subject_ != nullptr) {
        // 从主题获取相应的数据
        measurement_ = subject_->getMeasurements();
        // 执行相应的任务...
        display();
    }
}

void ConditionDisplayer::display()
{
    // Just for test.
    std::cout << "Current Conditions" << std::endl
              << "temperature:" << measurement_.temperature_ << "  "
              << "humidity:" << measurement_.humidity_ << " "
              << "pressure:" << measurement_.pressure_
              << std::endl;
}

// 单线程测试
// TODO: 多线程测试
int test_observer()
{
    std::shared_ptr<WeatherData> subjectPtr(new WeatherData());
    std::shared_ptr<ConditionDisplayer> observerPtr(new ConditionDisplayer(subjectPtr));
    subjectPtr->registerObserver(observerPtr);

    WeatherData::Measurement m1{ 20.0f, 12.0f, 45.0f };
    subjectPtr->setMeasurements(m1);

    WeatherData::Measurement m2{ 21.0f, 13.0f, 40.0f };
    subjectPtr->setMeasurements(m2);

    return 0;
}

