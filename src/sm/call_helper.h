#pragma once

#include <extensions/IBinTools.h>
#include <sm_argbuffer.h>

namespace sm {

    namespace detail {

        template<class T, class IsRef = typename std::is_reference<T>::type> struct CallTraitsFlag;
        template<class T> struct CallTraitsFlag<T, std::true_type> : std::integral_constant<int, PASSFLAG_BYREF> {};
        template<class T> struct CallTraitsFlag<T, std::false_type> : std::integral_constant<int, PASSFLAG_BYVAL> {};

        template<class T, class = typename std::is_pointer<typename std::decay<T>::type>::type, class = typename std::is_floating_point<T>::type, class = typename std::is_integral<T>::type, class = typename std::is_standard_layout<T>::type> class CallTraitsPassType;
        template<class T> struct CallTraitsPassType<T, std::true_type, std::false_type, std::false_type, std::true_type> : std::integral_constant<PassType, PassType_Basic> {};
        template<class T> struct CallTraitsPassType<T, std::false_type, std::false_type, std::true_type, std::true_type> : std::integral_constant<PassType, PassType_Basic> {};
        template<class T> struct CallTraitsPassType<T, std::false_type, std::true_type, std::false_type, std::true_type> : std::integral_constant<PassType, PassType_Float> {};
        template<class T> struct CallTraitsPassType<T, std::false_type, std::false_type, std::false_type, std::true_type> : std::integral_constant<PassType, PassType_Object> {};

        template<class T> PassInfo MakePassInfo()
        {
            PassInfo ret;
            ret.flags = CallTraitsFlag<T>::value;
            ret.type = CallTraitsPassType<T>::value;
            ret.size = sizeof(T);
            return ret;
        }

        template<> PassInfo MakePassInfo<void>()
        {
            return MakePassInfo<void *>();
        }


    }

    template<class Fn> class VFuncCaller;
    template<class C, class Ret, class...Args> class VFuncCaller<Ret(C::*)(Args...)> {
    public:
        VFuncCaller(IBinTools *bt, int offset, int vtblOffs = 0, int thisOffs = 0) : 
            m_pass{ detail::MakePassInfo<Args>()... }, 
            m_passRet(detail::MakePassInfo<Ret>()),
            m_pWrapper(bt->CreateVCall(offset, vtblOffs, thisOffs, std::is_void<Ret>::value ? nullptr : &m_passRet, m_pass, sizeof...(Args)))
        {}
        Ret operator()(C* pthis, const Args&...args)
        {
            ArgBuffer<void *, Args...> vstk(pthis, args...);
            typename std::conditional<std::is_void<Ret>::value, int, Ret>::type ret;
            m_pWrapper->Execute(vstk, std::is_void<Ret>::value ? nullptr : &ret);
            return Ret(ret);
        }
    protected:
        PassInfo m_pass[sizeof...(Args)];
        PassInfo m_passRet;
        ICallWrapper* m_pWrapper;
    };

    template<class Fn> class MemFuncCaller;
    template<class C, class Ret, class...Args> class MemFuncCaller<Ret(C::*)(Args...)> {
    public:
        MemFuncCaller(IBinTools *bt, void *addr) : 
            m_pass{ detail::MakePassInfo<Args>()... }, 
            m_passRet(detail::MakePassInfo<Ret>()),
            m_pWrapper( bt->CreateCall(addr, CallConv_ThisCall, std::is_void<Ret>::value ? nullptr : &m_passRet, m_pass, sizeof...(Args)) )
        {}

        Ret operator()(C *pthis, const Args&...args)
        {
            ArgBuffer<void *, Args...> vstk(pthis, args...);
            typename std::conditional<std::is_void<Ret>::value, int, Ret>::type ret;
            m_pWrapper->Execute(vstk, std::is_void<Ret>::value ? nullptr : &ret);
            return Ret(ret);
        }
    protected:
        PassInfo m_pass[sizeof...(Args)];
        PassInfo m_passRet;
        ICallWrapper* m_pWrapper;
    };
}

