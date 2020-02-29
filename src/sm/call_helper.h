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
        //template<class T> struct CallTraitsPassType<T, std::false_type, std::false_type, std::false_type, std::true_type> : std::integral_constant<PassType, PassType_Object> {};

        template<class T> PassInfo MakePassInfo()
        {
            PassInfo ret;
            ret.flags = CallTraitsFlag<T>::value;
            ret.type = CallTraitsPassType<T>::value;
            return ret;
        }

        template<class Ret> struct RetBufHelper;
        template<> struct RetBufHelper<void> {
            PassInfo *GetPassRetBuf() {
                return nullptr;
            }
            void CallWithRet(void *vParamStack) {
                this->m_pWrapper->Execute(vParamStack, nullptr);
            }
        protected:
            ICallWrapper *m_pWrapper;
        };
        template<class Ret> struct RetBufHelper : RetBufHelper<void> {
            PassInfo m_PassRet = MakePassInfo<Ret>();
            PassInfo *GetPassRetBuf() {
                return &m_PassRet;
            }
            Ret CallWithRet(void *vParamStack) {
                Ret ret;
                this->m_pWrapper->Execute(vParamStack, &ret);
                return ret;
            }
        };

        template<class Ret, class...Args> struct BaseCaller : RetBufHelper<Ret>{
        public:
            Ret operator()(const Args &...args)
            {
                ArgBuffer<Args...> vstk(args...);
                return this->CallWithRet(vstk);
            }
        };

    }

    template<class Fn> class VFuncCaller;
    template<class C, class Ret, class...Args> class VFuncCaller<Ret(C::*)(Args...)> : public detail::BaseCaller<Ret, C *, Args...> {
    public:
        VFuncCaller(IBinTools *bt, int offset, int vtblOffs = 0, int thisOffs = 0) {
            PassInfo pass[sizeof...(Args)] = { detail::MakePassInfo<Args>()... };
            this->m_pWrapper = bt->CreateVCall(offset, vtblOffs, thisOffs, this->GetPassRetBuf(), pass, sizeof...(Args));
        }
    };

    template<class Fn> class MemFuncCaller;
    template<class C, class Ret, class...Args> class MemFuncCaller<Ret(C::*)(Args...)> : public detail::BaseCaller<Ret, C *, Args...> {
    public:
        MemFuncCaller(IBinTools *bt, void *addr) {
            PassInfo pass[sizeof...(Args)] = { detail::MakePassInfo<Args>()... };
            this->m_pWrapper = bt->CreateCall(addr, CallConv_ThisCall, this->GetPassRetBuf(), pass, sizeof...(Args));
        }
    };
}

