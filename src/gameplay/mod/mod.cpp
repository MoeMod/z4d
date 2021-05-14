
#include "extension.h"
#include "mod.h"
#include <string_view>

#include "mod_none.h"
#include "mod_zp.h"

namespace gameplay {
	namespace mod {
		std::shared_ptr<IBaseMod> g_pModRunning;

		std::shared_ptr<IBaseMod> ModFactory(std::string_view sv)
		{
			if (sv == "zp")
				return std::make_shared<Mod_ZP>();

			return std::make_shared<Mod_None>();
		}

		void Init()
		{
			ICommandLine* cmd = gamehelpers->GetValveCommandLine();
			if (auto kv = cmd->ParmValue("+mod", ""))
			{
				g_pModRunning = ModFactory(kv);
			}
		}
	}
}
