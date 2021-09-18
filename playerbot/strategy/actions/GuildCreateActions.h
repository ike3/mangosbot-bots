#pragma once

#include "../Action.h"
#include "InventoryAction.h"
#include "ChooseTravelTargetAction.h"
#include "../values/BudgetValues.h"

namespace ai
{
    class BuyPetitionAction : public InventoryAction {
    public:
        BuyPetitionAction(PlayerbotAI* ai) : InventoryAction(ai, "buy petition") {}
        virtual bool Execute(Event event);
        virtual bool isUseful();
        static bool canBuyPetition(Player* bot);
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
        virtual bool isUseful() { return !bot->GetGuildId() && AI_VALUE2(uint32, "item count", chat->formatQItem(5863)); };
    };

    class PetitionTurnInAction : public ChooseTravelTargetAction {
    public:
        PetitionTurnInAction(PlayerbotAI* ai) : ChooseTravelTargetAction(ai, "petition turn in") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return !bot->GetGuildId() && AI_VALUE2(uint32, "item count", chat->formatQItem(5863)) && AI_VALUE(uint8, "petition signs") >= sWorld.getConfig(CONFIG_UINT32_MIN_PETITION_SIGNS) && !context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling(); };
    };

    class BuyTabardAction : public ChooseTravelTargetAction {
    public:
        BuyTabardAction(PlayerbotAI* ai) : ChooseTravelTargetAction(ai, "buy tabard") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return bot->GetGuildId() && !AI_VALUE2(uint32, "item count", chat->formatQItem(5976)) && AI_VALUE2(uint32, "free money for", uint32(NeedMoneyFor::guild)) >= 10000 && !context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling(); };
    };
}
