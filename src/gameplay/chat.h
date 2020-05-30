#pragma once
#include <string>

namespace gameplay {
	namespace chat {
		bool OnClientSay(int id, std::string str, bool team);
	}
}