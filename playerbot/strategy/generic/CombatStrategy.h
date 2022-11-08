#pragma once

namespace ai
{
    class CombatStrategy : public Strategy
    {
    public:
        CombatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual void InitTriggers(std::list<TriggerNode*> &triggers) override;
        virtual int GetType() override { return STRATEGY_TYPE_COMBAT; }
    };

    class AvoidAoeStrategy : public Strategy
    {
    public:
        AvoidAoeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "avoid aoe"; }
        void InitMultipliers(std::list<Multiplier*>& multipliers) override;
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class AvoidAoeStrategyMultiplier : public Multiplier
    {
    public:
        AvoidAoeStrategyMultiplier(PlayerbotAI* ai) : Multiplier(ai, "run away on area debuff") {}
        float GetValue(Action* action) override;
    };

    class WaitForAttackStrategy : public Strategy
    {
    public:
        WaitForAttackStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "wait for attack"; }
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
        void InitMultipliers(std::list<Multiplier*>& multipliers) override;

        static bool ShouldWait(PlayerbotAI* ai);
        static uint8 GetWaitTime(PlayerbotAI* ai);
        static float GetSafeDistance() { return sPlayerbotAIConfig.spellDistance; }
        static float GetSafeDistanceThreshold() { return 2.5f; }
    };

    class WaitForAttackMultiplier : public Multiplier
    {
    public:
        WaitForAttackMultiplier(PlayerbotAI* ai) : Multiplier(ai, "wait for for attack") {}
        float GetValue(Action* action) override;
    };
}
