#pragma once

#include "../Action.h"
#include "InventoryAction.h"
#include "ChooseTravelTargetAction.h"

namespace ai
{
    class PetitionSignAction : public Action {
    public:
        PetitionSignAction(PlayerbotAI* ai) : Action(ai, "petition sign") {}
        virtual bool Execute(Event event);
    };

    class PetitionOfferAction : public Action {
    public:
        PetitionOfferAction(PlayerbotAI* ai, string name = "petition offer") : Action(ai, name) {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return !bot->GetGuildId(); };
    };

    class PetitionOfferNearbyAction : public PetitionOfferAction {
    public:
        PetitionOfferNearbyAction(PlayerbotAI* ai) : PetitionOfferAction(ai, "petition offer nearby") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return !bot->GetGuildId() && bot->GetItemByEntry(5863); };
    };

    class PetitionTurnInAction : public ChooseTravelTargetAction {
    public:
        PetitionTurnInAction(PlayerbotAI* ai) : ChooseTravelTargetAction(ai, "petition turn in") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return !bot->GetGuildId() && bot->GetItemByEntry(5863) && AI_VALUE(uint8, "petition signs") >= sWorld.getConfig(CONFIG_UINT32_MIN_PETITION_SIGNS) && !context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling(); };
    };
}
