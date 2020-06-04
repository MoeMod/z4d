#pragma once

#include "util/EventDispatcher.hpp"
#include <IForwardSys.h>

namespace sm {
    inline namespace functions {
        namespace detail {

            template<class T> struct TypeIdentity
            {
                template<class...Args> constexpr T operator()(Args&&...) const noexcept { return T{}; }
            };

            struct Plugin_Continue_t : std::integral_constant<ResultType, Pl_Continue>, TypeIdentity<Plugin_Continue_t> {};
            struct Plugin_Changed_t : std::integral_constant<ResultType, Pl_Changed>, TypeIdentity<Plugin_Changed_t> {};
            struct Plugin_Handled_t : std::integral_constant<ResultType, Pl_Handled>, TypeIdentity<Plugin_Handled_t> {};
            struct Plugin_Stop_t : std::integral_constant<ResultType, Pl_Stop>, TypeIdentity<Plugin_Stop_t> {};
        }

        constexpr detail::Plugin_Continue_t Plugin_Continue;
        constexpr detail::Plugin_Changed_t Plugin_Changed;
        constexpr detail::Plugin_Handled_t Plugin_Handled;
        constexpr detail::Plugin_Stop_t Plugin_Stop;

        template<class T> struct HookResult;
        template<> struct HookResult<void> {
            ResultType value;
            constexpr HookResult() : HookResult(Plugin_Continue) {}
            constexpr explicit HookResult(ResultType v) : value(v) {}
            constexpr HookResult(decltype(Plugin_Continue) v) : value(v) {}
            constexpr HookResult(decltype(Plugin_Changed) v) : value(v) {}
            constexpr HookResult(decltype(Plugin_Handled) v) : value(v) {}
            constexpr HookResult(decltype(Plugin_Stop) v) : value(v) {}
            constexpr operator ResultType() const { return value; }
        };
        template<class T> struct HookResult : HookResult<void> {
            using Base = HookResult<void>;
            constexpr HookResult() : HookResult(Plugin_Continue) {}
            constexpr HookResult(decltype(Plugin_Continue) v) : Base(v) {}
            HookResult(decltype(Plugin_Changed) v, T x) : Base(v), retval(std::move(x)) {}
            HookResult(decltype(Plugin_Handled) v) : Base(v) {}
            HookResult(decltype(Plugin_Stop) v, T x) : Base(v), retval(std::move(x)) {}

            ~HookResult() {
                if (Base::value == Plugin_Changed || Base::value == Plugin_Stop)
                    retval.ret.~T();
            }

            union OptionalResult{
                T ret;
                bool ph;

                OptionalResult() : ph(false) {}
                OptionalResult(T x) : ret(std::move(x)) {}
            } retval;
        };
        using Action = HookResult<void>;

        template<class R, class...Args, class...RealArgs>
        R CallForwardEvent(EventDispatcher<R(Args...)>& f, RealArgs&&...args)
        {
            return f.dispatch_reduce(std::forward<RealArgs>(args)..., [](auto a, auto b) { return std::max(a, b); }, R(Plugin_Continue));
        }
        template<class R, class...Args, class...RealArgs>
        R CallForwardHook(EventDispatcher<R(Args...)>& f, RealArgs&&...args)
        {
            return f.dispatch_find_if(std::forward<RealArgs>(args)..., [](auto v) { return v == Pl_Stop; }, R(Plugin_Continue));
        }
        template<class R, class...Args, class...RealArgs>
        R CallForwardSingle(EventDispatcher<R(Args...)>& f, RealArgs&&...args)
        {
            return f.dispatch_reduce(std::forward<RealArgs>(args)..., [](auto a, auto b) { return b; }, R(Plugin_Continue));
        }
        template<class R, class...Args, class...RealArgs>
        auto CallForwardIgnore(EventDispatcher<R(Args...)>& f, RealArgs&&...args)
        {
            f.dispatch(std::forward<RealArgs>(args)...);
            return Plugin_Continue;
        }
    }
}