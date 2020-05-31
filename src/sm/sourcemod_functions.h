#pragma once

#include <IForwardSys.h>
#include <array>

#include "util/ThinkQueue.h"

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

			inline auto GetForwardFunctionCount(IForward* forward)
			{
				return forward->GetFunctionCount();
			}

			template<class...ArgTypes>
			[[nodiscard]] std::shared_ptr<IForward> CreateGlobalForwardRAII(const char* name, SourceMod::ExecType type, ArgTypes...args)
			{
				return std::shared_ptr<IForward>(CreateGlobalForward(name, type, args...), (void(*)(IForward*))CloseHandle);
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