#pragma once
#include "../Trigger.h"

namespace ai
{
    class NoRpgTargetTrigger : public Trigger
    {
    public:
        NoRpgTargetTrigger(PlayerbotAI* ai, string name = "no rpg target", int checkInterval = 1) : Trigger(ai, name, checkInterval) {}

        virtual bool IsActive() { return !AI_VALUE(GuidPosition, "rpg target"); };
    };

    class HasRpgTargetTrigger : public NoRpgTargetTrigger
    {
    public:
        HasRpgTargetTrigger(PlayerbotAI* ai, string name = "has rpg target", int checkInterval = 2) : NoRpgTargetTrigger(ai, name, checkInterval) {}

        virtual bool IsActive() { return !NoRpgTargetTrigger::IsActive() && AI_VALUE(string, "next rpg action") != "choose rpg target"; }; //Ingore rpg targets that only have the cancel action available.
    };

    class FarFromRpgTargetTrigger : public NoRpgTargetTrigger
    {
    public:
        FarFromRpgTargetTrigger(PlayerbotAI* ai, string name = "far from rpg target", int checkInterval = 1) : NoRpgTargetTrigger(ai, name, checkInterval) {}

        virtual bool IsActive() { return !NoRpgTargetTrigger::IsActive() && AI_VALUE2(float, "distance", "rpg target") > INTERACTION_DISTANCE; };
    };

    class NearRpgTargetTrigger : public FarFromRpgTargetTrigger
    {
    public:
        NearRpgTargetTrigger(PlayerbotAI* ai, string name = "near rpg target", int checkInterval = 1) : FarFromRpgTargetTrigger(ai, name, checkInterval) {}

        virtual bool IsActive() { return !NoRpgTargetTrigger::IsActive() && !FarFromRpgTargetTrigger::IsActive(); };
    };

    //Sub actions:
    class RpgTrigger : public FarFromRpgTargetTrigger
    {
    public:
        RpgTrigger(PlayerbotAI* ai, string name = "sub rpg", int checkInterval = 2) : FarFromRpgTargetTrigger(ai, name, checkInterval) {}

        GuidPosition getGuidP() { return AI_VALUE(GuidPosition, "rpg target"); }

        virtual bool IsActive() { return !ai->HasRealPlayerMaster() || (AI_VALUE(GuidPosition, "rpg target").GetEntry() && AI_VALUE(GuidPosition, "rpg target").GetEntry() == AI_VALUE(TravelTarget*, "travel target")->getEntry()); };
        virtual Event Check() { if (!NoRpgTargetTrigger::IsActive() && (AI_VALUE(string, "next rpg action") == "choose rpg target" || !FarFromRpgTargetTrigger::IsActive())) return Trigger::Check(); return Event(); };
    };

    class RpgTaxiTrigger : public RpgTrigger
    {
    public:
        RpgTaxiTrigger(PlayerbotAI* ai, string name = "rpg taxi") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgDiscoverTrigger : public RpgTrigger
    {
    public:
        RpgDiscoverTrigger(PlayerbotAI* ai, string name = "rpg discover") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgStartQuestTrigger : public RpgTrigger
    {
    public:
        RpgStartQuestTrigger(PlayerbotAI* ai, string name = "rpg start quest") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgEndQuestTrigger : public RpgTrigger
    {
    public:
        RpgEndQuestTrigger(PlayerbotAI* ai, string name = "rpg end quest") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgRepeatQuestTrigger : public RpgTrigger
    {
    public:
        RpgRepeatQuestTrigger(PlayerbotAI* ai, string name = "rpg repeat quest") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgBuyTrigger : public RpgTrigger
    {
    public:
        RpgBuyTrigger(PlayerbotAI* ai, string name = "rpg buy") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgSellTrigger : public RpgTrigger
    {
    public:
        RpgSellTrigger(PlayerbotAI* ai, string name = "rpg sell") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgAHSellTrigger : public RpgTrigger
    {
    public:
        RpgAHSellTrigger(PlayerbotAI* ai, string name = "rpg ah sell") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgAHBuyTrigger : public RpgTrigger
    {
    public:
        RpgAHBuyTrigger(PlayerbotAI* ai, string name = "rpg ah buy") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgGetMailTrigger : public RpgTrigger
    {
    public:
        RpgGetMailTrigger(PlayerbotAI* ai, string name = "rpg get mail") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgRepairTrigger : public RpgTrigger
    {
    public:
        RpgRepairTrigger(PlayerbotAI* ai, string name = "rpg repair") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgTrainTrigger : public RpgTrigger
    {
    public:
        RpgTrainTrigger(PlayerbotAI* ai, string name = "rpg train") : RpgTrigger(ai, name) {}

        static bool IsTrainerOf(CreatureInfo const* cInfo, Player* pPlayer);

        virtual bool IsActive();
    };

    class RpgHealTrigger : public RpgTrigger
    {
    public:
        RpgHealTrigger(PlayerbotAI* ai, string name = "rpg heal") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgHomeBindTrigger : public RpgTrigger
    {
    public:
        RpgHomeBindTrigger(PlayerbotAI* ai, string name = "rpg home bind") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgQueueBGTrigger : public RpgTrigger
    {
    public:
        RpgQueueBGTrigger(PlayerbotAI* ai, string name = "rpg queue bg") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgBuyPetitionTrigger : public RpgTrigger
    {
    public:
        RpgBuyPetitionTrigger(PlayerbotAI* ai, string name = "rpg buy petition") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgUseTrigger : public RpgTrigger
    {
    public:
        RpgUseTrigger(PlayerbotAI* ai, string name = "rpg use") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgSpellTrigger : public RpgTrigger
    {
    public:
        RpgSpellTrigger(PlayerbotAI* ai, string name = "rpg spell") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgCraftTrigger : public RpgTrigger
    {
    public:
        RpgCraftTrigger(PlayerbotAI* ai, string name = "rpg craft") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };

    class RpgTradeUsefulTrigger : public RpgTrigger
    {
    public:
        RpgTradeUsefulTrigger(PlayerbotAI* ai, string name = "rpg trade useful") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    private:
        virtual bool isFriend(Player* player); //Move to value later.
    };

    class RpgDuelTrigger : public RpgTrigger
    {
    public:
        RpgDuelTrigger(PlayerbotAI* ai, string name = "rpg duel") : RpgTrigger(ai, name) {}
        virtual bool IsActive();
    };
}
