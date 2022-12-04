#pragma once
#include "../Multiplier.h"
#include "../Strategy.h"

namespace ai
{
    class RpgActionMultiplier : public Multiplier
    {
    public:
        RpgActionMultiplier(PlayerbotAI* ai) : Multiplier(ai, "rpg action") {}

    public:
        virtual float GetValue(Action* action);
    };

    class RpgStrategy : public Strategy
    {
    public:
        RpgStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual string getName() override { return "rpg"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "rpg"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy makes bot move between npcs to automatically do various interaction.\n"
            "This is the main rpg strategy which make bots pickand move to various rpg targets.\n"
            "The interactions included in this strategy are limited to emotesand wait.";            
        }
        virtual vector<string> GetRelatedStrategies() { return {"rpg quest", "rpg vendor", "rpg explore", "rpg maintenance", "rpg guild", "rpg bg", "rpg player", "rpg craft"}; }
#endif
    protected:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        virtual void InitNonCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };

    class RpgQuestStrategy : public RpgStrategy
    {
    public:
        RpgQuestStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg quest"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgVendorStrategy : public RpgStrategy
    {
    public:
        RpgVendorStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg vendor"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgExploreStrategy : public RpgStrategy
    {
    public:
        RpgExploreStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg explore"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgMaintenanceStrategy : public RpgStrategy
    {
    public:
        RpgMaintenanceStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg maintenance"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgGuildStrategy : public RpgStrategy
    {
    public:
        RpgGuildStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg guild"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgBgStrategy : public RpgStrategy
    {
    public:
        RpgBgStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg bg"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgPlayerStrategy : public RpgStrategy
    {
    public:
        RpgPlayerStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg player"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RpgCraftStrategy : public RpgStrategy
    {
    public:
        RpgCraftStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        string getName() override { return "rpg craft"; }

    private:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}