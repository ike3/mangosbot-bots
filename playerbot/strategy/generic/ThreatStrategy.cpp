#include "botpch.h"
#include "../../playerbot.h"
#include "ThreatStrategy.h"
#include "../../PlayerbotAIConfig.h"
#include "../actions/GenericSpellActions.h"

using namespace ai;

float ThreatMultiplier::GetValue(Action* action)
{
    if (action == NULL || action->getThreatType() == ActionThreatType::ACTION_THREAT_NONE)
        return 1.0f;

    if (!AI_VALUE(bool, "group"))
        return 1.0f;

    if (action->getThreatType() == ActionThreatType::ACTION_THREAT_AOE)
    {
        uint8 threat = AI_VALUE2(uint8, "threat", "aoe");
        if (threat >= 50)
            return 0.0f;
    }    

    uint8 relativeThreat = AI_VALUE2(uint8, "threat", "current target");

    if (ai->HasStrategy("debug threat", BotState::BOT_STATE_COMBAT))
    {
        if (ai->GetMaster())
        {
            if (relativeThreat > 100)
            {
                ai->GetMaster()->GetSession()->SendPlaySpellVisual(ai->GetBot()->GetObjectGuid(), 6372);
            }
            else if (relativeThreat > 80)
            {
                ai->GetMaster()->GetSession()->SendPlaySpellVisual(ai->GetBot()->GetObjectGuid(), 5036);
            }
        }
    }

    if (relativeThreat >= 80)
    {
        //Watch delta.
        uint32 lastTime = MEM_AI_VALUE(float, "my threat::current target")->GetLastTime();

        if (lastTime < time(0))
        {
            float deltaThreat = LOG_AI_VALUE(float, "my threat::current target")->GetDelta(5.0f);
            float currentThreat = AI_VALUE2(float, "my threat", "current target");

            if (deltaThreat > 0)
            {
                float tankThreat = AI_VALUE2(float, "tank threat", "current target");

                float newThreat = currentThreat + deltaThreat * 5.0f; //No agro in 5 seconds.

                if (newThreat < tankThreat)
                    return 1.0f;
            }
        }

        return 0.0f;
    }

    return 1.0f;
}

void ThreatStrategy::InitCombatMultipliers(std::list<Multiplier*> &multipliers)
{
    multipliers.push_back(new ThreatMultiplier(ai));
}
