#pragma once
#include "../Trigger.h"

namespace ai
{	
    class PetitionTurnInTrigger : public Trigger {
    public:
        PetitionTurnInTrigger(PlayerbotAI* ai) :
            Trigger(ai) {}

        bool IsActive() { return !bot->GetGuildId() && bot->GetItemByEntry(5863) && AI_VALUE(uint8, "petition signs") >= sWorld.getConfig(CONFIG_UINT32_MIN_PETITION_SIGNS); };
    };

    class LeaveLargeGuildTrigger : public Trigger {
    public:
        LeaveLargeGuildTrigger(PlayerbotAI* ai) :
            Trigger(ai) {}

        bool IsActive();
    };
}