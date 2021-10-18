#include "botpch.h"
#include "../../playerbot.h"
#include "ThreatValues.h"

#include "../../ServerFacade.h"
#include "ThreatManager.h"

using namespace ai;

uint8 ThreatValue::Calculate()
{
    if (qualifier == "aoe")
    {
        uint8 maxThreat = 0;
        list<ObjectGuid> attackers = context->GetValue<list<ObjectGuid> >("attackers")->Get();
        for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); i++)
        {
            Unit* unit = ai->GetUnit(*i);
            if (!unit || !sServerFacade.IsAlive(unit))
                continue;

            uint8 threat = Calculate(unit);
            if (!maxThreat || threat > maxThreat)
                maxThreat = threat;
        }

        return maxThreat;
    }

    Unit* target = AI_VALUE(Unit*, qualifier);
    return Calculate(target);
}

uint8 ThreatValue::Calculate(Unit* target)
{
    if (!target)
        return 0;

    if (target->GetObjectGuid().IsPlayer())
        return 0;

    Group* group = bot->GetGroup();
    if (!group)
        return 0;

    float botThreat = sServerFacade.GetThreatManager(target).getThreat(bot);
    float maxThreat = -1.0f;
    bool hasTank = false;

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* player = gref->getSource();
        if (!player || !sServerFacade.IsAlive(player) || player == bot)
            continue;

        if (ai->IsTank(player))
        {
            hasTank = true;
            float threat = sServerFacade.GetThreatManager(target).getThreat(player);
            if (maxThreat < threat)
                maxThreat = threat;
        }
    }

    if (maxThreat <= 0 && !hasTank)
        return 0;

    // calculate normal threat for fleeing targets
    bool fleeing = target->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLEEING_MOTION_TYPE ||
        target->GetMotionMaster()->GetCurrentMovementGeneratorType() == TIMED_FLEEING_MOTION_TYPE;

    // return high threat if tank has no threat
    if (target->IsInCombat() && maxThreat <= 0 && botThreat <= 0 && hasTank && !fleeing)
        return 100;

    // return low threat if mob if fleeing
    if (hasTank && fleeing)
        return 0;

    return botThreat * 100 / maxThreat;
}
