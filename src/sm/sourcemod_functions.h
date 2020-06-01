#pragma once

#include <IForwardSys.h>
#include <array>

#include "util/ThinkQueue.h"
#include "interop.h"

extern IForwardManager* g_pForwards;

namespace sm {
	inline namespace sourcemod {
		inline namespace functions {

			template<class...ArgTypes>
			IForward* CreateGlobalForward(const char* name, SourceMod::ExecType type, ArgTypes...args)
			{
				std::array<SourceMod::ParamType, sizeof...(ArgTypes)> il = { args... };
				IForward* result = g_pForwards->CreateForward(name, type, il.size(), il.data());
				return result;
			}

			inline void CloseHandle(IForward* forward)
			{
				if (forward)
					g_pForwards->ReleaseForward(forward);
			}

			template<class PtrToForward = IForward *>
			inline auto GetForwardFunctionCount(const PtrToForward &forward)
			{
				return forward->GetFunctionCount();
			}

			template<class...ArgTypes>
			[[nodiscard]] std::shared_ptr<IForward> CreateGlobalForwardRAII(const char* name, SourceMod::ExecType type, ArgTypes...args)
			{
				return std::shared_ptr<IForward>(CreateGlobalForward(name, type, args...), (void(*)(IForward*))CloseHandle);
			}

			namespace detail {
				template<class T> struct type_identity {};
				constexpr std::integral_constant<SourceMod::ParamType, Param_Cell> GetParamType(cell_t) { return {}; }
				constexpr std::integral_constant<SourceMod::ParamType, Param_CellByRef> GetParamType(cell_t&) { return {}; }
				constexpr std::integral_constant<SourceMod::ParamType, Param_Float> GetParamType(float) { return {}; }
				constexpr std::integral_constant<SourceMod::ParamType, Param_FloatByRef> GetParamType(float&) { return {}; }
				constexpr std::integral_constant<SourceMod::ParamType, Param_String> GetParamType(std::string_view) { return {}; }
				template<class ArrayType> 
				constexpr auto GetParamType(ArrayType&& arr) -> decltype(std::begin(arr), std::end(arr), std::integral_constant<SourceMod::ParamType, Param_Array>()) {
					return {};
				}
				template<class Ret, class...Args> 
				auto CreateGlobalForwardFunc_impl(const char* name, SourceMod::ExecType type, type_identity<Ret(Args...)> type_ext)
				{
					std::shared_ptr<IForward> fwd = CreateGlobalForwardRAII(name, type, decltype(GetParamType(std::declval<Args>()))::value...);
					return interop::ForwardCaller<Ret(Args...), std::shared_ptr<IForward>>(fwd);
				}
			}

			template<class FnType = void()> 
			[[nodiscard]] auto CreateGlobalForwardFunc(const char* name, SourceMod::ExecType type)
			{
				return detail::CreateGlobalForwardFunc_impl(name, type, detail::type_identity<FnType>());
			}

			extern ThinkQueue g_ThinkQueue;

			// Extra : Thread-safe to call.
			template<class Fn, class...Args>
			void RequestFrame(Fn&& fn, Args&&...data)
			{
				g_ThinkQueue.AddTask(std::forward<Fn>(fn), std::forward<Args>(data)...);
			}

			template<class Fn, class...Args>
			void RunOnMainThread(Fn&& fn, Args&&...data)
			{
				return sm::RequestFrame(std::forward<Fn>(fn), std::forward<Args>(data)...);
			}
		}
	}
}