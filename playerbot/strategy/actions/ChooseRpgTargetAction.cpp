#include "botpch.h"
#include "../../playerbot.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool ChooseRpgTargetAction::Execute(Event event)
{
    list<ObjectGuid> possibleTargets = AI_VALUE(list<ObjectGuid>, "possible rpg targets");
    if (possibleTargets.empty())
    {
        return false;
    }

    set<ObjectGuid>& ignore = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

    vector<Unit*> units;
    for (list<ObjectGuid>::iterator i = possibleTargets.begin(); i != possibleTargets.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && (ignore.empty() || ignore.find(unit->GetObjectGuid()) == ignore.end())) units.push_back(unit);
    }

    if (units.empty())
    {
        sLog.outDetail("%s can't choose RPG target: all %zu are not available", bot->GetName(), possibleTargets.size());
        ignore.clear(); //Clear ignore list.
        context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignore);
        return false;
    }



    Unit* target = units[urand(0, units.size() - 1)];
    if (!target) {
        return false;
    }

    context->GetValue<ObjectGuid>("rpg target")->Set(target->GetObjectGuid());

    if (ignore.size() > 50)
        ignore.erase(ignore.begin());

    ignore.insert(target->GetObjectGuid());

    
    context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignore);

    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    return !context->GetValue<ObjectGuid>("rpg target")->Get();
}
