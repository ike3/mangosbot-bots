#include "botpch.h"
#include "../../playerbot.h"
#include "SetHomeAction.h"
#include "../../PlayerbotAIConfig.h"


using namespace ai;

bool SetHomeAction::Execute(Event event)
{
    Player* master = GetMaster();

    ObjectGuid selection = bot->GetSelectionGuid();
    bool isRpgAction = AI_VALUE(GuidPosition, "rpg target") == selection;

    if (!isRpgAction)
        if (master)
            selection = master->GetSelectionGuid();
        else
            return false;

    if (selection)
    {
        Unit* unit = ai->GetUnit(selection);
        if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_INNKEEPER))
        {
            if (isRpgAction)
            {
                Creature* creature = ai->GetCreature(selection);                   
                bot->GetSession()->SendBindPoint(creature);
                ai->TellMaster("This inn is my new home");
                RESET_AI_VALUE(WorldPosition, "home bind");
                return true;
            }
            else
            {
                Creature* creature = ai->GetCreature(selection);
                bot->GetSession()->SendBindPoint(creature);
                ai->TellMaster("This inn is my new home");
                RESET_AI_VALUE(WorldPosition, "home bind");
                return true;
            }
        }
    }

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Creature *unit = bot->GetNPCIfCanInteractWith(*i, UNIT_NPC_FLAG_INNKEEPER);
        if (!unit)
            continue;

        bot->GetSession()->SendBindPoint(unit);
        ai->TellMaster("This inn is my new home");
        RESET_AI_VALUE(WorldPosition, "home bind");
        return true;
    }

    ai->TellError("Can't find any innkeeper around");
    return false;
}
