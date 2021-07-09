#include "botpch.h"
#include "../../playerbot.h"
#include "SnareTargetValue.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "MotionGenerators/TargetedMovementGenerator.h"

using namespace ai;

Unit* SnareTargetValue::Calculate()
{
    string spell = qualifier;

    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("attackers")->Get();
    Unit* target = ai->GetAiObjectContext()->GetValue<Unit*>("current target")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit)
            continue;

        if (bot->GetDistance(unit) > ai->GetRange("spell"))
            continue;

        Unit* chaseTarget;
        switch (unit->GetMotionMaster()->GetCurrentMovementGeneratorType())
        {
        case FLEEING_MOTION_TYPE:
            return unit;
        case CHASE_MOTION_TYPE:
            chaseTarget = sServerFacade.GetChaseTarget(unit);
            if (!chaseTarget) continue;
            Player* chaseTargetPlayer = sObjectMgr.GetPlayer(chaseTarget->GetObjectGuid());
            
            // check if need to snare
            bool shouldSnare = true;

            // do not slow down if bot is melee and mob/bot attack each other
            if (chaseTargetPlayer && !ai->IsRanged(bot) && chaseTargetPlayer == bot)
                shouldSnare = false;

            if (!sServerFacade.isMoving(unit))
                shouldSnare = false;

            if (unit->HasAuraType(SPELL_AURA_MOD_ROOT))
                shouldSnare = false;

            if (chaseTargetPlayer && shouldSnare && !ai->IsTank(chaseTargetPlayer)) {
                return unit;
            }
        }
    }

    return NULL;
}
