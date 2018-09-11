#include "botpch.h"
#include "../../playerbot.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool ChooseRpgTargetAction::Execute(Event event)
{
    list<ObjectGuid> possibleTargets = AI_VALUE(list<ObjectGuid>, "possible rpg targets");
    if (possibleTargets.empty()) return false;

    vector<Unit*> units;
    for (list<ObjectGuid>::iterator i = possibleTargets.begin(); i != possibleTargets.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && sServerFacade.GetDistance2d(bot, unit) > sPlayerbotAIConfig.tooCloseDistance) units.push_back(unit);
    }

    if (units.empty())
        return false;

    Unit* target = units[urand(0, units.size() - 1)];

    sLog.outString("%s has choosen RPG target: %s (%d/%d available)", bot->GetName(), target->GetName(), units.size(), possibleTargets.size());
    context->GetValue<Unit*>("rpg target")->Set(target);
    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    return !context->GetValue<Unit*>("rpg target")->Get();
}
