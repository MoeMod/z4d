#pragma once

#include <atomic>

class ThinkQueue
{
public:
    template<class Fn>
    void AddTask(Fn &&fn)
    {
        BaseTask * task = new Task<typename std::decay<Fn>::type>(std::forward<Fn>(fn));
        AddTask(task);
    }

    void CallAndClear();


private:
    class BaseTask
    {
    public:
        virtual ~BaseTask() = default;
        virtual void operator()() = 0;

        BaseTask *next = nullptr;
    };

    template<class Fn>
    class Task : public BaseTask
    {
    public:
        template<class FnArg>
        explicit Task(FnArg &&f) : m_func(std::forward<FnArg>(f)) {}
        Task(const Task &) = delete;
        Task(Task &&) = delete;

        void operator()() override {
            return m_func();
        }
        Fn m_func;
    };

    void AddTask(BaseTask * task);

    std::atomic<BaseTask *> head = nullptr;
};