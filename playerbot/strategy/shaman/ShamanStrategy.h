#pragma once
#include "../generic/ClassStrategy.h"

namespace ai
{
    class ShamanAoePlaceholderStrategy : public AoePlaceholderStrategy
    {
    public:
        ShamanAoePlaceholderStrategy(PlayerbotAI* ai) : AoePlaceholderStrategy(ai) {}
        string getName() override { return "aoe"; }
    };

    class ShamanCurePlaceholderStrategy : public CurePlaceholderStrategy
    {
    public:
        ShamanCurePlaceholderStrategy(PlayerbotAI* ai) : CurePlaceholderStrategy(ai) {}
        string getName() override { return "cure"; }
    };

    class ShamanTotemsPlaceholderStrategy : public PlaceholderStrategy
    {
    public:
        ShamanTotemsPlaceholderStrategy(PlayerbotAI* ai) : PlaceholderStrategy(ai) {}
        string getName() override { return "totems"; }
    };

    class ShamanStrategy : public ClassStrategy
    {
    public:
        ShamanStrategy(PlayerbotAI* ai);

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanPvpStrategy : public ClassPvpStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitReactionTriggers(std::list<TriggerNode*>& triggers);
        static void InitDeadTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanPveStrategy : public ClassPveStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitReactionTriggers(std::list<TriggerNode*>& triggers);
        static void InitDeadTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanAoeStrategy : public AoeStrategy
    {
    public:
        ShamanAoeStrategy(PlayerbotAI* ai) : AoeStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanAoePvpStrategy : public AoePvpStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanAoePveStrategy : public AoePveStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanBuffDpsStrategy : public Strategy
    {
    public:
        ShamanBuffDpsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bdps"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanBuffManaStrategy : public Strategy
    {
    public:
        ShamanBuffManaStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bmana"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanCureStrategy : public CureStrategy
    {
    public:
        ShamanCureStrategy(PlayerbotAI* ai) : CureStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanCurePvpStrategy : public CurePvpStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanCurePveStrategy : public CurePveStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanTotemsStrategy : public Strategy
    {
    public:
        ShamanTotemsStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanTotemsPvpStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanTotemsPveStrategy
    {
    public:
        static void InitCombatTriggers(std::list<TriggerNode*>& triggers);
        static void InitNonCombatTriggers(std::list<TriggerNode*>& triggers);
    };

    class ShamanTotemBarElementsStrategy : public Strategy
    {
    public:
        ShamanTotemBarElementsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() { return "totembar elements"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanTotemBarAncestorsStrategy : public Strategy
    {
    public:
        ShamanTotemBarAncestorsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() { return "totembar ancestors"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanTotemBarSpiritsStrategy : public Strategy
    {
    public:
        ShamanTotemBarSpiritsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() { return "totembar spirits"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
