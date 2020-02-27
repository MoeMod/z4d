#pragma once

#include "util/EventDispatcher.hpp"

namespace sm {

    using ::META_RES;

    constexpr struct Ignored_t : std::integral_constant<META_RES, MRES_IGNORED> {} Ignored;
    constexpr struct Handled_t : std::integral_constant<META_RES, MRES_HANDLED> {} Handled;
    constexpr struct Override_t : std::integral_constant<META_RES, MRES_OVERRIDE> {} Override;
    constexpr struct Supercede_t : std::integral_constant<META_RES, MRES_SUPERCEDE> {} Supercede;

    template<class T> struct HookResult;
    template<> struct HookResult<void> {
        META_RES value;
        constexpr HookResult(Ignored_t v) : value(v) {}
        constexpr HookResult(Handled_t v) : value(v) {}
        constexpr HookResult(Override_t v) : value(v) {}
        constexpr HookResult(Supercede_t v) : value(v) {}
    };
    template<class T> struct HookResult : HookResult<void> {
        using Base = HookResult<void>;
        constexpr HookResult(Ignored_t v) : Base(v) {}
        constexpr HookResult(Handled_t v) : Base(v) {}
        HookResult(Override_t v, T x) : Base(v), retval(std::move(x)) {}
        HookResult(Supercede_t v, T x) : Base(v), retval(std::move(x)) {}

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

}