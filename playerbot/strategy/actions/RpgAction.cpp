#include "botpch.h"
#include "../../playerbot.h"
#include "RpgAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "GossipDef.h"

using namespace ai;

bool RpgAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "rpg target");
    if (!target)
    {
        return false;
    }

    if (sServerFacade.isMoving(bot))
    {
        return false;
    }

    if (bot->GetMapId() != target->GetMapId())
    {
        context->GetValue<Unit*>("rpg target")->Set(NULL);
        return false;
    }

    if (!sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT) && !bot->IsTaxiFlying())
    {
        sServerFacade.SetFacingTo(bot, target);
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
        return false;
    }

    if (!bot->GetNPCIfCanInteractWith(target->GetObjectGuid(), UNIT_NPC_FLAG_NONE))
    {
        return false;
    }

    vector<RpgElement> elements;
    elements.push_back(&RpgAction::cancel);
    elements.push_back(&RpgAction::talk);
    elements.push_back(&RpgAction::stay);
    elements.push_back(&RpgAction::cry);
    elements.push_back(&RpgAction::rude);
    elements.push_back(&RpgAction::point);

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
    if (bot->PlayerTalkClass) bot->PlayerTalkClass->CloseGossip();
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
    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(3000);
}

void RpgAction::cancel(Unit* unit)
{
    context->GetValue<Unit*>("rpg target")->Set(NULL);
}

bool RpgAction::isUseful()
{
    return context->GetValue<Unit*>("rpg target")->Get();
}
