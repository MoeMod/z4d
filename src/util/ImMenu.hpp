#pragma once

#include "smhelper.h"
#include <vector>
#include <string>
#include <chrono>

namespace util {
	inline namespace smhelper {
		class ImMenuContext
		{
		public:
			virtual bool begin(const std::string& title) = 0;
			virtual void enabled() = 0;
			virtual void disabled() = 0;
			virtual bool item(const std::string& info, const std::string& text) = 0;
			virtual bool end(int client, std::chrono::seconds time = {}) = 0;
		protected:
			virtual ~ImMenuContext() = 0;
		};
		inline ImMenuContext::~ImMenuContext() = default;

		namespace detail {
			class ImMenuBuilder : public ImMenuContext
			{
			public:
				ImMenuBuilder(IBaseMenu* m) : menu(m), next_disabled(false) {}

				bool begin(const std::string& title) override
				{
					menu->RemoveAllItems();
					menu->SetDefaultTitle(title.c_str());
					return true;
				}

				void enabled() override
				{
					next_disabled = false;
				}

				void disabled() override
				{
					next_disabled = true;
				}

				bool item(const std::string& info, const std::string& text) override
				{
					menu->AppendItem(info.c_str(), ItemDrawInfo(text.c_str(), std::exchange(next_disabled, false) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT));
					return false;
				}

				bool end(int client, std::chrono::seconds time = {}) override
				{
					using namespace std::chrono_literals;
					menu->Display(client, time / 1s);
					return false;
				}

			private:
				IBaseMenu* const menu;
				bool next_disabled;
			};

			class ImMenuCaller : public ImMenuContext
			{
			public:
				ImMenuCaller(IBaseMenu* m, unsigned item)
					: menu(m), next_disabled(false), selected_item(m->GetItemInfo(item, nullptr))
				{

				}

				bool begin(const std::string& title) override
				{
					return true;
				}

				void enabled() override
				{
					next_disabled = false;
				}

				void disabled() override
				{
					next_disabled = true;
				}

				bool item(const std::string& info, const std::string& text) override
				{
					return !std::exchange(next_disabled, false) && info == selected_item;
				}

				bool end(int voter, std::chrono::seconds time = {}) override
				{
					return true;
				}

			private:
				IBaseMenu* const menu;
				bool next_disabled;
				const std::string selected_item;
			};
		}

		template<class Fn = void(*)(ImMenuContext&&)>
		auto ImMenu(Fn &&fn) -> typename std::enable_if<std::is_invocable<Fn, detail::ImMenuCaller>::value&& std::is_invocable<Fn, detail::ImMenuBuilder>::value>::type
		{
			using SourceMod::IBaseMenu;
			auto menu = util::MakeMenu([fn](IBaseMenu* menu, int id, unsigned int item) {
				fn(detail::ImMenuCaller(menu, item));
				});
			fn(detail::ImMenuBuilder(menu.get()));
		}
	}
}