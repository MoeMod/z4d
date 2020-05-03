#pragma once

#include <memory>
#include <IMenuManager.h>

namespace util {
    inline namespace smhelper {

        using SourceMod::IBaseMenu;
        // void onSelected(IBaseMenu *menu, int client, unsigned int item)
        template<class Fn> [[nodiscard]] std::shared_ptr<IBaseMenu> MakeMenu(Fn &&onSelected)
        {
            class MenuHandler : public IMenuHandler {
            public:
                explicit MenuHandler(Fn fn, IBaseMenu*menu = nullptr) : m_fnOnSelect(std::move(fn)), m_pMenu(menu) {}
                void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override
                {
                    return m_fnOnSelect(menu, client, item);
                }
                void OnMenuEnd(IBaseMenu* menu, MenuEndReason reason) override
                {
                    // delete this !
                    assert(m_pSharedThis.get() == this);
                    auto spThis = std::exchange(m_pSharedThis, nullptr);
                }
                ~MenuHandler()
                {
                    assert(m_pSharedThis == nullptr);
                }
                Fn m_fnOnSelect;
                IBaseMenu* m_pMenu;
                std::shared_ptr<MenuHandler> m_pSharedThis;
            };
            auto handler = std::make_shared<MenuHandler>(std::forward<Fn>(onSelected));
            handler->m_pMenu = menus->GetDefaultStyle()->CreateMenu(handler.get());
            handler->m_pSharedThis = handler; // self-cycle
            return std::shared_ptr<IBaseMenu>(handler, handler->m_pMenu);
        }

        template<class Fn> [[nodiscard]] std::shared_ptr<ITimer> SetTask(float fInterval, Fn &&onTick, int flags = 0)
        {
            class MyHandler : public ITimedEvent {
            public:
                explicit MyHandler(Fn fn, ITimer*p = nullptr) : m_fn(std::move(fn)), m_pTimer(p) {}
                ResultType OnTimer(ITimer *pTimer, void *pData) override
                {
                    m_fn();
                    return Pl_Continue;
                }
                void OnTimerEnd(ITimer *pTimer, void *pData) override
                {
                    // delete this !
                    assert(m_pSharedThis.get() == this);
                    auto spThis = std::exchange(m_pSharedThis, nullptr);
                }
                ~MyHandler()
                {
                    assert(m_pTimer != nullptr);
                    if(m_pSharedThis != nullptr)
                        timersys->KillTimer(m_pTimer);
                }
                Fn m_fn;
                ITimer* m_pTimer;
                std::shared_ptr<MyHandler> m_pSharedThis;
            };
            auto handler = std::make_shared<MyHandler>(std::forward<Fn>(onTick));
            handler->m_pTimer = timersys->CreateTimer(handler.get(), fInterval, nullptr, flags);
            handler->m_pSharedThis = handler; // self-cycle
            return std::shared_ptr<ITimer>(handler, handler->m_pTimer);
        }
    }
}