#pragma once
#include "../generic/CombatStrategy.h"

namespace ai
{
    class GenericRogueStrategy : public CombatStrategy
    {
    public:
        GenericRogueStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "rogue"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DpsRogueStrategy : public GenericRogueStrategy
    {
    public:
        DpsRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai) {}
        string getName() override { return "dps"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class AssassinationRogueStrategy : public GenericRogueStrategy
    {
    public:
        AssassinationRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai) {}
        string getName() override { return "assassin"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class CombatRogueStrategy : public GenericRogueStrategy
    {
    public:
        CombatRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai) {}
        string getName() override { return "combat"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class SubtletyRogueStrategy : public GenericRogueStrategy
    {
    public:
        SubtletyRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai) {}
        string getName() override { return "subtlety"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class StealthedRogueStrategy : public Strategy
    {
    public:
        StealthedRogueStrategy(PlayerbotAI* ai);
        string getName() override { return "stealthed"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class StealthStrategy : public Strategy
    {
    public:
        StealthStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "stealth"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class RogueAoeStrategy : public Strategy
    {
    public:
        RogueAoeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class RogueBoostStrategy : public Strategy
    {
    public:
        RogueBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "boost"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class RogueCcStrategy : public Strategy
    {
    public:
        RogueCcStrategy(PlayerbotAI* ai);
        string getName() override { return "cc"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
