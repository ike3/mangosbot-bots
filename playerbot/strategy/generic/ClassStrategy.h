#pragma once
#include "../Strategy.h"

namespace ai
{
    class ClassStrategy : public Strategy
    {
    public:
        ClassStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitReactionTriggers(std::list<TriggerNode *>& triggers) override;
        virtual void InitDeadTriggers(std::list<TriggerNode *>& triggers) override;
    };

    class ClassPvpStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitReactionTriggers(std::list<TriggerNode*>& triggers);
        static void InitDeadTriggers(std::list<TriggerNode*>& triggers);
    };

    class ClassPveStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitReactionTriggers(std::list<TriggerNode*>& triggers);
        static void InitDeadTriggers(std::list<TriggerNode*>& triggers);
    };

    class ClassBossStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitReactionTriggers(std::list<TriggerNode*>& triggers);
        static void InitDeadTriggers(std::list<TriggerNode*>& triggers);
    };

    class AoeStrategy : public Strategy
    {
    public:
        AoeStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class AoePvpStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class AoePveStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class AoeBossStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class CureStrategy : public Strategy
    {
    public:
        CureStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class CurePvpStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class CurePveStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class CureBossStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class BoostStrategy : public Strategy
    {
    public:
        BoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BoostPvpStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class BoostPveStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class BoostBossStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class CcStrategy : public Strategy
    {
    public:
        CcStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class CcPvpStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class CcPveStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class CcBossStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    // This is a strategy to be used only as a placeholder
    class PlaceholderStrategy : public Strategy
    {
    public:
        PlaceholderStrategy(PlayerbotAI* ai) : Strategy(ai) {}
    };

    // This strategy is used to hold the bot spec strategy
    class SpecPlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        SpecPlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    
    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // This strategy is used to hold the aoe strategy
    class AoePlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        AoePlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    };

    // This strategy is used to hold the cure strategy
    class CurePlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        CurePlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    };

    // This strategy is used to hold the boost strategy
    class BoostPlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        BoostPlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    };

    // This strategy is used to hold the cc strategy
    class CcPlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        CcPlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    };
}