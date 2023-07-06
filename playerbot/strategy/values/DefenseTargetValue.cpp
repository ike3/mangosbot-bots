#include "botpch.h"
#include "../../playerbot.h"
#include "DefenseTargetValue.h"

#include "AttackersValue.h"
using namespace ai;

class FindTargetForDefenseStrategy : public FindNonCcTargetStrategy
{
public:
    FindTargetForDefenseStrategy(PlayerbotAI* ai) : FindNonCcTargetStrategy(ai)
    {
        maxThreat = 0;
    }

public:
    virtual void CheckAttacker(Unit* creature, ThreatManager* threatManager)
    {
        Player* bot = ai->GetBot();
        if (IsCcTarget(creature)) return;

        HostileReference* ref = threatManager->getCurrentVictim();
        if (!ref || ref->getTarget() != bot) return;

        float threat = threatManager->getThreat(bot);
        if (!result || (maxThreat - threat) < 0.1f)
        {
            maxThreat = threat;
            result = creature;
        }
    }

protected:
    float maxThreat;
};


Unit* DefenseTargetValue::Calculate()
{
    FindTargetForDefenseStrategy strategy(ai);
    return FindTarget(&strategy);
}
