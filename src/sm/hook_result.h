#pragma once

#include "util/EventDispatcher.hpp"
#include <sourcehook.h>

namespace sm {

    using ::META_RES;

    constexpr struct : std::integral_constant<META_RES, MRES_IGNORED> {} Ignored;
    constexpr struct : std::integral_constant<META_RES, MRES_HANDLED> {} Handled;
    constexpr struct : std::integral_constant<META_RES, MRES_OVERRIDE> {} Override;
    constexpr struct : std::integral_constant<META_RES, MRES_SUPERCEDE> {} Supercede;

    template<class T> struct HookResult;
    template<> struct HookResult<void> {
        META_RES value;
        constexpr HookResult() : HookResult(Ignored) {}
        constexpr HookResult(decltype(Ignored) v) : value(v) {}
        constexpr HookResult(decltype(Handled) v) : value(v) {}
        constexpr HookResult(decltype(Override) v) : value(v) {}
        constexpr HookResult(decltype(Supercede) v) : value(v) {}
        constexpr operator META_RES() const { return value; }
    };
    template<class T> struct HookResult : HookResult<void> {
        using Base = HookResult<void>;
        constexpr HookResult() : HookResult(Ignored) {}
        constexpr HookResult(decltype(Ignored) v) : Base(v) {}
        constexpr HookResult(decltype(Handled) v) : Base(v) {}
        HookResult(decltype(Override) v, T x) : Base(v), retval(std::move(x)) {}
        HookResult(decltype(Supercede) v, T x) : Base(v), retval(std::move(x)) {}

        HookResult(T x) : HookResult(Supercede, std::move(x)) {}

        ~HookResult() {
            if (Base::value == Override || Base::value == Supercede)
                retval.ret.~T();
        }

        union OptionalResult{
            T ret;
            bool ph;

            OptionalResult() : ph(false) {}
            OptionalResult(T x) : ret(std::move(x)) {}
        } retval;
    };


    constexpr bool ShouldContinue(META_RES value) { return value != MRES_IGNORED; }
    constexpr bool ShouldBlock(META_RES value) { return !ShouldContinue(value); }
}