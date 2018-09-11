#include "botpch.h"
#include "../../playerbot.h"
#include "RpgAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool RpgAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "rpg target");
    if (!target) return false;

    Creature* creature = bot->GetNPCIfCanInteractWith(target->GetObjectGuid(), UNIT_NPC_FLAG_NONE);
    if (!creature) return false;

    WorldObject* faceTo = target;
    if (!sServerFacade.IsInFront(bot, faceTo, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT) && !bot->IsTaxiFlying())
    {
        bot->SetFacingTo(bot->GetAngle(faceTo));
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
        return false;
    }

    vector<RpgElement> elements;

    for (int i = 0; i < 10; i++)
    {
        elements.push_back(&RpgAction::stay);
        elements.push_back(&RpgAction::cancel);
    }

    for (int i = 0; i < 50; i++)
    {
        elements.push_back(&RpgAction::talk);
    }

    for (int i = 0; i < 2; i++)
    {
        elements.push_back(&RpgAction::cry);
        elements.push_back(&RpgAction::beg);
        elements.push_back(&RpgAction::rude);
        elements.push_back(&RpgAction::point);
    }

    RpgElement element = elements[urand(0, elements.size() - 1)];
    (this->*element)(target);
    return true;
}

void RpgAction::longemote(Unit* unit, uint32 type)
{
    emote(unit, type);
    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::stay(Unit* unit)
{
    sLog.outDetail("%s is doing RPG action: stay", bot->GetName());
    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::emote(Unit* unit, uint32 type)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    WorldPacket p1;
    p1 << unit->GetObjectGuid();
    bot->GetSession()->HandleGossipHelloOpcode(p1);

    bot->HandleEmoteCommand(type);

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    sLog.outString("%s is doing RPG action: emote %u", bot->GetName(), type);

    ai->SetNextCheckDelay(3000);
}

void RpgAction::cancel(Unit* unit)
{
    sLog.outDetail("%s has dropped RPG target %s", bot->GetName(), unit->GetName());
    context->GetValue<Unit*>("rpg target")->Set(NULL);
}

bool RpgAction::isUseful()
{
    return context->GetValue<Unit*>("rpg target")->Get();
}
