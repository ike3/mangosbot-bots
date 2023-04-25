#pragma once
#include "../Strategy.h"

namespace ai
{
    // Generic strategy to be used for all class rotations
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

    class ClassRaidStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitReactionTriggers(std::list<TriggerNode*>& triggers);
        static void InitDeadTriggers(std::list<TriggerNode*>& triggers);
    };

    // Generic strategy to be used for all class aoe rotations
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

    class AoeRaidStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    // Generic strategy to be used for all class cure rotations
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

    class CureRaidStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    // Generic strategy to be used for all class cc rotations
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

    class CcRaidStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    // Generic strategy to be used for all class buff rotations 
    // (combat for Boost CDs and non combat for buffs)
    class BuffStrategy : public Strategy
    {
    public:
        BuffStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BuffPvpStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class BuffPveStrategy
    {
    protected:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class BuffRaidStrategy
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

    // This strategy is used to hold the cc strategy
    class CcPlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        CcPlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    };

    // This strategy is used to hold the buff strategy
    class BuffPlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        BuffPlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
    };
}