#include "extension.h"
#include "event.h"

#undef CreateEvent
#include <IGameEvents.h>

#include "sm/sourcemod.h"

namespace gameplay {
	namespace death {
		void DeathCreateIcon(int userID, int attackerID, const char* sIcon, bool bHead = false, bool bPenetrated = false, bool bRevenge = false, bool bDominated = false, int assisterID = 0)
		{
			if (IGameEvent* ev = event::gameevents->CreateEvent("player_death"))
			{
				ev->SetInt("userid", userID);
				ev->SetInt("attacker", attackerID);
				ev->SetInt("assister", assisterID);
				ev->SetString("weapon", sIcon);
				ev->SetBool("headshot", bHead);
				ev->SetBool("penetrated", bPenetrated);
				ev->SetBool("revenge", bRevenge);
				ev->SetBool("dominated", bDominated);

				for (int i = 1; i <= playerhelpers->GetMaxClients(); i++)
				{
					// Send fake event
					IGamePlayer *igp = sm::IGamePlayerFrom(i);
					
				}

				event::gameevents->FireEvent(ev);
				event::gameevents->FreeEvent(ev);
			}
		}
	}
}