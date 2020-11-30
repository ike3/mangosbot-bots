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
        return false;

    string ignore = context->GetValue<string>("ignore rpg target")->Get();

    vector<Unit*> units;
    for (list<ObjectGuid>::iterator i = possibleTargets.begin(); i != possibleTargets.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && (ignore.empty() || ignore.find(to_string(unit->GetObjectGuid())) == string::npos)) units.push_back(unit);
    }

    if (units.empty())
    {
        sLog.outDetail("%s can't choose RPG target: all %zu are not available", bot->GetName(), possibleTargets.size());
        return false;
    }

    Unit* target = units[urand(0, units.size() - 1)];
    if (!target) return false;

    context->GetValue<ObjectGuid>("rpg target")->Set(target->GetObjectGuid());

    if (ignore.size() > 1000)
        ignore = ignore.replace(0, ignore.find(",") + 1, "");

    ignore = ignore + to_string(target->GetObjectGuid()) + ",";

    context->GetValue<string>("ignore rpg target")->Set(ignore);

    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    return !context->GetValue<ObjectGuid>("rpg target")->Get();
}
