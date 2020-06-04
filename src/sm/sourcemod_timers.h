
#include <memory>
#include <ITimerSystem.h>

namespace sm {
	inline namespace timers {
        #define TIMER_REPEAT TIMER_FLAG_REPEAT /**< Timer will repeat until it returns Plugin_Stop */
        // TIMER_FLAG_NO_MAPCHANGE /**< Timer will not carry over mapchanges */

        // concept TimerFunc
        // void func();
        // void func(std::shared_ptr<ITimer>);
        // void func(std::shared_ptr<ITimer>, AnyDataType);
        namespace detail {
            template<std::size_t N> struct priority_tag : priority_tag<N-1> {};
            template<> struct priority_tag<0> {};
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler &rthis, priority_tag<3>)
                -> decltype(fn())
            {
                return fn();
            }
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler& rthis, priority_tag<2>)
                -> decltype(fn(std::declval<std::shared_ptr<ITimer>>()))
            {
                return fn(std::shared_ptr<ITimer>(rthis.m_pSharedThis, rthis.m_pTimer));
            }
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler& rthis, priority_tag<1>)
                -> decltype(fn(std::declval<std::shared_ptr<ITimer>>(), std::declval<typename std::decay<MyHandler>::type::data_type>()))
            {
                return fn(std::shared_ptr<ITimer>(rthis.m_pSharedThis, rthis.m_pTimer), rthis.m_data);
            }
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler& rthis)
            {
                return CallTimerFunc(fn, rthis, priority_tag<4>());
            }
        }

        template<class Fn, class AnyDataType = std::nullptr_t> 
        std::weak_ptr<ITimer> CreateTimer(float fInterval, Fn&& func, AnyDataType &&data = nullptr, int flags = 0)
        {
            class MyHandler : public ITimedEvent {
            public:
                using data_type = AnyDataType;
                explicit MyHandler(Fn fn, ITimer* p, AnyDataType &&data) : m_fn(std::move(fn)), m_pTimer(p), m_data(std::forward<AnyDataType>(data)) {}
                ResultType OnTimer(ITimer* pTimer, void* pData) override
                {
                    detail::CallTimerFunc(m_fn, *this);
                    return Pl_Stop;
                }
                void OnTimerEnd(ITimer* pTimer, void* pData) override
                {
                    // delete this !
                    assert(m_pSharedThis.get() == this);
                    auto spThis = std::exchange(m_pSharedThis, nullptr);
                }
                ~MyHandler()
                {
                    assert(m_pTimer != nullptr);
                    if (m_pSharedThis != nullptr)
                        timersys->KillTimer(m_pTimer);
                }
                Fn m_fn;
                ITimer* m_pTimer;
                std::shared_ptr<MyHandler> m_pSharedThis;
                AnyDataType m_data;
            };
            auto handler = std::make_shared<MyHandler>(std::forward<Fn>(func), nullptr, std::forward<AnyDataType>(data));
            handler->m_pTimer = timersys->CreateTimer(handler.get(), fInterval, nullptr, flags);
            handler->m_pSharedThis = handler; // self-cycle
            return std::shared_ptr<ITimer>(handler, handler->m_pTimer);
        }

        inline void KillTimer(const std::weak_ptr<ITimer> &timer)
        {
            if(auto sp = timer.lock())
                timersys->KillTimer(sp.get());
        }

        inline void TriggerTimer(const std::weak_ptr<ITimer> &timer, bool reset = false)
        {
            if (auto sp = timer.lock())
                timersys->FireTimerOnce(sp.get(), reset);
        }

        // Not implemeted. 
        template<class Fn>
        std::weak_ptr<ITimer> CreateDataTimer(float fInterval, Fn&& func, Handle_t& datapack, int flags = 0) = delete;

        template<class Fn, class AnyDataType = std::nullptr_t>
        std::shared_ptr<ITimer> CreateTimerRAII(float fInterval, Fn&& func, AnyDataType&& data = nullptr, int flags = 0)
        {
            auto sp = CreateTimer(fInterval, std::forward<Fn>(func), data, flags).lock();
            return std::shared_ptr<ITimer>(sp.get(), [sp](ITimer* timer) { timersys->KillTimer(timer); });
        }
	}
}