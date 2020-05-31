#pragma once

#include <atomic>
#include <tuple>

class ThinkQueue
{
public:
    template<class Fn, class...BoundArgs>
    void AddTask(Fn &&fn, BoundArgs&&...args)
    {
        BaseTask * task = new Task<typename std::decay<Fn>::type, typename std::decay<BoundArgs>::type...>(std::forward<Fn>(fn), std::forward<BoundArgs>(args)...);
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

    template<class Fn, class...BoundArgs>
    class Task : public BaseTask
    {
    public:
        template<class FnArg, class...Args>
        explicit Task(FnArg &&f, Args &&...args) : m_func(std::forward<FnArg>(f)), m_args(std::forward<Args>(args)...) {}
        Task(const Task &) = delete;
        Task(Task &&) = delete;

        void operator()() override {
            return std::apply(m_func, m_args);
        }
        Fn m_func;
        std::tuple<BoundArgs...> m_args;
    };

    void AddTask(BaseTask * task);

    std::atomic<BaseTask *> head = nullptr;
};