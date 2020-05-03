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
				ImMenuBuilder(IBaseMenu* m) : menu(m) {}

				bool begin(const std::string& title) override
				{
					menu->RemoveAllItems();
					menu->SetDefaultTitle(title.c_str());
					return true;
				}

				bool item(const std::string& info, const std::string& text) override
				{
					menu->AppendItem(info.c_str(), ItemDrawInfo(text.c_str()));
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
			};

			class ImMenuCaller : public ImMenuContext
			{
			public:
				ImMenuCaller(IBaseMenu* m, unsigned item)
					: menu(m), selected_item(m->GetItemInfo(item, nullptr))
				{

				}

				bool begin(const std::string& title) override
				{
					return true;
				}

				bool item(const std::string& info, const std::string& text) override
				{
					return info == selected_item;
				}

				bool end(int voter, std::chrono::seconds time = {}) override
				{
					return true;
				}

			private:
				IBaseMenu* const menu;
				const std::string selected_item;
			};
		}

		// void fn(ImMenuContext &)
		template<class Fn>
		void ImMenu(Fn fn)
		{
			static_assert(std::is_invocable<Fn, detail::ImMenuCaller>::value && std::is_invocable<Fn, detail::ImMenuBuilder>::value,
				"Fn should be void(ImMenuContext &&) or void(auto)");
			using SourceMod::IBaseMenu;
			auto menu = util::MakeMenu([fn](IBaseMenu* menu, int id, unsigned int item) {
				fn(detail::ImMenuCaller(menu, item));
				});
			fn(detail::ImMenuBuilder(menu.get()));
		}
	}
}