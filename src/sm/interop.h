
#include <algorithm>
#include <vector>
#include <string>
#include <functional>

#include <sp_vm_api.h>
#include <Color.h>
#include <Vector.h>

namespace sm {
	namespace interop {
		// decl
		template<class Fn> class PluginFunctionCaller;

		// bool => bool
		// char => char
		// int => int / cell_t
		// float => float
		// Handle => Handle_t
		// string => std::string
		// array => std::vector / std::array
		// Function => PluginFunctionCaller / std::function
		
		template<class T>
		inline auto cell2native(IPluginContext* pContext, cell_t in, T &out) -> typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, int>::type
		{
			out = static_cast<T>(in);
			return 1;
		}

		template<class T>
		inline auto cell2native(IPluginContext* pContext, cell_t in, T& out) -> typename std::enable_if<std::is_floating_point<T>::value, int>::type
		{
			out = sp_ctof(in);
			return 1;
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, std::string& out)
		{
			char* str; pContext->LocalToString(in, &str);
			assert(str != nullptr);
			out = str;
			return 1;
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, Color& out)
		{
			cell_t* vecParams;
			pContext->LocalToPhysAddr(in, &vecParams);
			out = Color(vecParams[0], vecParams[1], vecParams[2]);
			return 1;
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, Vector& out)
		{
			cell_t* vecParams;
			pContext->LocalToPhysAddr(in, &vecParams);
			out = Vector(sp_ctof(vecParams[0]), sp_ctof(vecParams[1]), sp_ctof(vecParams[2]));
			return 1;
		}

		template<template<class> class FnType, class Ret, class...Args >
		inline auto cell2native(IPluginContext* pContext, cell_t in, FnType<Ret(Args...)>& out)
			-> typename std::enable_if<
				std::is_invocable_r<Ret, FnType<Ret(Args...)>, Args...>::value && 
				std::is_assignable< FnType<Ret(Args...)>, PluginFunctionCaller<Ret(Args...)> >::value
			, int>::type
		{
			IPluginFunction* pFunction = pContext->GetFunctionById(in);
			if (!pFunction)
			{
				return pContext->ThrowNativeError("Invalid function id (%X)", in);
			}
			out = PluginFunctionCaller<Ret(Args...)>(pFunction);
			return 1;
		}

		inline cell_t n2c(bool in) { return !!in; }
		inline cell_t n2c(cell_t in) { return in; }
		inline cell_t n2c(float in) { return sp_ftoc(in); }

		inline void func_push(ICallable* c, cell_t x) { c->PushCell(x); }
		inline void func_push(ICallable* c, float x) { c->PushFloat(x); }
		template<class ArrayType> auto func_push(ICallable* c, const ArrayType& arr) -> decltype(std::begin(arr), std::end(arr), void())
		{
			std::vector<cell_t> in;
			std::transform(std::begin(arr), std::end(arr), std::back_inserter(in), [](const auto& x) { return n2c(x); });
			c->PushArray(in.data(), in.size());
		}
		inline void func_push(ICallable* c, const std::string &str)
		{
			c->PushString(str.c_str());
		}
		inline void func_push(ICallable* c, const char *psz)
		{
			c->PushString(psz);
		}

		template<class Fn> class PluginFunctionCaller;
		template<class Ret, class...Args> class PluginFunctionCaller<Ret(Args...)>
		{
			IPluginFunction* const m_pfn;
		public:
			PluginFunctionCaller(IPluginFunction* pf) : m_pfn(pf) {}
			Ret operator()(const Args &...args) const
			{
				cell_t result;
				(func_push(m_pfn, args), ..., m_pfn->Execute(&result));
				if constexpr (!std::is_void_v<Ret>)
				{
					Ret ret;
					cell2native(m_pfn->GetParentContext(), &result, ret);
					return ret;
				}
			}
#if __cplusplus >= 202001
			friend auto operator<=>(const PluginFunctionCaller &a, const PluginFunctionCaller &b)
			{
				return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
			}
#else
#define OPFN(OP) \
			friend bool operator OP (const PluginFunctionCaller& a, const PluginFunctionCaller& b) \
			{ \
				return a.m_pfn->GetFunctionID() OP b.m_pfn->GetFunctionID(); \
			} 
		OPFN(==)
		OPFN(!=)
		OPFN(<)
		OPFN(>)
		OPFN(<=)
		OPFN(>=)
#undef OPFN
#endif
		};

		template<class...Args>
		void params2vars(IPluginContext* pContext, const cell_t* params, Args&...out)
		{
			int N = 1;
			((N += cell2native(pContext, params[N], out)), ...);
		}

		template<class TupleType, std::size_t...I>
		void params2tuple_impl(IPluginContext* pContext, const cell_t* params, TupleType& ret, std::index_sequence<I...>)
		{
			params2vars(pContext, params, std::get<I>(ret)...);
		}

		template<class...Args>
		std::tuple<Args...> params2tuple(IPluginContext* pContext, const cell_t* params)
		{
			std::tuple<Args...> ret;
			params2tuple_impl(pContext, params, ret, std::index_sequence_for<Args...>());
			return ret;
		}
	}
}