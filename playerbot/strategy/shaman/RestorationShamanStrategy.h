#pragma once
#include "ShamanStrategy.h"

namespace ai
{
    class RestorationShamanPlaceholderStrategy : public SpecPlaceholderStrategy
    {
    public:
        RestorationShamanPlaceholderStrategy(PlayerbotAI* ai) : SpecPlaceholderStrategy(ai) {}
        string getName() override { return "restoration"; }
    };

    class RestorationShamanStrategy : public ShamanStrategy
    {
    public:
        RestorationShamanStrategy(PlayerbotAI* ai);

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanPveStrategy : public RestorationShamanStrategy
    {
    public:
        RestorationShamanPveStrategy(PlayerbotAI* ai) : RestorationShamanStrategy(ai) {}
        std::string getName() override { return "restoration pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanPvpStrategy : public RestorationShamanStrategy
    {
    public:
        RestorationShamanPvpStrategy(PlayerbotAI* ai) : RestorationShamanStrategy(ai) {}
        std::string getName() override { return "restoration pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanAoeStrategy : public ShamanAoeStrategy
    {
    public:
        RestorationShamanAoeStrategy(PlayerbotAI* ai) : ShamanAoeStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanAoePveStrategy : public RestorationShamanAoeStrategy
    {
    public:
        RestorationShamanAoePveStrategy(PlayerbotAI* ai) : RestorationShamanAoeStrategy(ai) {}
        string getName() override { return "aoe restoration pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanAoePvpStrategy : public RestorationShamanAoeStrategy
    {
    public:
        RestorationShamanAoePvpStrategy(PlayerbotAI* ai) : RestorationShamanAoeStrategy(ai) {}
        string getName() override { return "aoe restoration pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanCureStrategy : public ShamanCureStrategy
    {
    public:
        RestorationShamanCureStrategy(PlayerbotAI* ai) : ShamanCureStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanCurePveStrategy : public RestorationShamanCureStrategy
    {
    public:
        RestorationShamanCurePveStrategy(PlayerbotAI* ai) : RestorationShamanCureStrategy(ai) {}
        string getName() override { return "cure restoration pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanCurePvpStrategy : public RestorationShamanCureStrategy
    {
    public:
        RestorationShamanCurePvpStrategy(PlayerbotAI* ai) : RestorationShamanCureStrategy(ai) {}
        string getName() override { return "cure restoration pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanTotemsStrategy : public ShamanTotemsStrategy
    {
    public:
        RestorationShamanTotemsStrategy(PlayerbotAI* ai) : ShamanTotemsStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanTotemsPveStrategy : public RestorationShamanTotemsStrategy
    {
    public:
        RestorationShamanTotemsPveStrategy(PlayerbotAI* ai) : RestorationShamanTotemsStrategy(ai) {}
        string getName() override { return "totems restoration pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanTotemsPvpStrategy : public RestorationShamanTotemsStrategy
    {
    public:
        RestorationShamanTotemsPvpStrategy(PlayerbotAI* ai) : RestorationShamanTotemsStrategy(ai) {}
        string getName() override { return "totems restoration pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
