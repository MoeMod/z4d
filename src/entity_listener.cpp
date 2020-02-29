#include "extension.h"

#include <ISmmPlugin.h>
#include <extensions/ISDKHooks.h>

#include "entity_listener.h"
#include "sm/sdkhooks.h"

namespace entity_listener {

    class EntityListener : public ISMEntityListener
    {
    public:
        void OnEntityCreated(CBaseEntity *pEntity, const char *classname) override
        {

        }

        void OnEntityDestroyed(CBaseEntity *pEntity) override
        {

        }
    } g_EntityListener;

    ISDKHooks *g_pSDKHooks = NULL;

    bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late)
    {
        return true;
    }

    bool SDK_OnLoad(char *error, size_t maxlength, bool late)
    {
        sharesys->AddDependency(myself, "sdkhooks.ext", true, true);

        SM_GET_IFACE(SDKHOOKS, g_pSDKHooks);

        g_pSDKHooks->AddEntityListener(&g_EntityListener);

        return true;
    }

    void SDK_OnUnload()
    {
        g_pSDKHooks->RemoveEntityListener(&g_EntityListener);
    }
}