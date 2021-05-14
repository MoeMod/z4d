#pragma once

namespace gameplay {
	namespace death {
		void DeathCreateIcon(int userID, int attackerID, const char* sIcon, bool bHead = false, bool bPenetrated = false, bool bRevenge = false, bool bDominated = false, int assisterID = 0);
	}
}