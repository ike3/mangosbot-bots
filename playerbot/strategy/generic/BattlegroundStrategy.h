#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class BGStrategy : public PassTroughStrategy
    {
    public:
        BGStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai) {}
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
        string getName() override { return "bg"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override {}
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override {}
    };

    class BattlegroundStrategy : public Strategy
    {
    public:
        BattlegroundStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
        string getName() override { return "battleground"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class WarsongStrategy : public Strategy
    {
    public:
        WarsongStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_GENERIC; }
        string getName() override { return "warsong"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class AlteracStrategy : public Strategy
    {
    public:
        AlteracStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_GENERIC; }
        string getName() override { return "alterac"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ArathiStrategy : public Strategy
    {
    public:
        ArathiStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_GENERIC; }
        string getName() override { return "arathi"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EyeStrategy : public Strategy
    {
    public:
        EyeStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_GENERIC; }
        string getName() { return "eye"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class IsleStrategy : public Strategy
    {
    public:
        IsleStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_GENERIC; }
        string getName() override { return "isle"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ArenaStrategy : public Strategy
    {
    public:
        ArenaStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        int GetType() override { return STRATEGY_TYPE_GENERIC; }
        string getName() override { return "arena"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
