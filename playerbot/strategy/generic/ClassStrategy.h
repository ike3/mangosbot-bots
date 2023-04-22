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
}