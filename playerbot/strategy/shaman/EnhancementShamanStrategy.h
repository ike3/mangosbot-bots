#pragma once
#include "ShamanStrategy.h"

namespace ai
{
    class EnhancementShamanPlaceholderStrategy : public SpecPlaceholderStrategy
    {
    public:
        EnhancementShamanPlaceholderStrategy(PlayerbotAI* ai) : SpecPlaceholderStrategy(ai) {}
        string getName() override { return "enhancement"; }
    };

    class EnhancementShamanStrategy : public ShamanStrategy
    {
    public:
        EnhancementShamanStrategy(PlayerbotAI* ai);

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;

        virtual NextAction** GetDefaultCombatActions() override;
    };

    class EnhancementShamanPveStrategy : public EnhancementShamanStrategy
    {
    public:
        EnhancementShamanPveStrategy(PlayerbotAI* ai) : EnhancementShamanStrategy(ai) {}
        std::string getName() override { return "enhancement pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanPvpStrategy : public EnhancementShamanStrategy
    {
    public:
        EnhancementShamanPvpStrategy(PlayerbotAI* ai) : EnhancementShamanStrategy(ai) {}
        std::string getName() override { return "enhancement pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanBossStrategy : public EnhancementShamanStrategy
    {
    public:
        EnhancementShamanBossStrategy(PlayerbotAI* ai) : EnhancementShamanStrategy(ai) {}
        std::string getName() override { return "enhancement boss"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanAoeStrategy : public ShamanAoeStrategy
    {
    public:
        EnhancementShamanAoeStrategy(PlayerbotAI* ai) : ShamanAoeStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanAoePveStrategy : public EnhancementShamanAoeStrategy
    {
    public:
        EnhancementShamanAoePveStrategy(PlayerbotAI* ai) : EnhancementShamanAoeStrategy(ai) {}
        string getName() override { return "aoe enhancement pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanAoePvpStrategy : public EnhancementShamanAoeStrategy
    {
    public:
        EnhancementShamanAoePvpStrategy(PlayerbotAI* ai) : EnhancementShamanAoeStrategy(ai) {}
        string getName() override { return "aoe enhancement pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanAoeBossStrategy : public EnhancementShamanAoeStrategy
    {
    public:
        EnhancementShamanAoeBossStrategy(PlayerbotAI* ai) : EnhancementShamanAoeStrategy(ai) {}
        string getName() override { return "aoe enhancement boss"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanCureStrategy : public ShamanCureStrategy
    {
    public:
        EnhancementShamanCureStrategy(PlayerbotAI* ai) : ShamanCureStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanCurePveStrategy : public EnhancementShamanCureStrategy
    {
    public:
        EnhancementShamanCurePveStrategy(PlayerbotAI* ai) : EnhancementShamanCureStrategy(ai) {}
        string getName() override { return "cure enhancement pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanCurePvpStrategy : public EnhancementShamanCureStrategy
    {
    public:
        EnhancementShamanCurePvpStrategy(PlayerbotAI* ai) : EnhancementShamanCureStrategy(ai) {}
        string getName() override { return "cure enhancement pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanCureBossStrategy : public EnhancementShamanCureStrategy
    {
    public:
        EnhancementShamanCureBossStrategy(PlayerbotAI* ai) : EnhancementShamanCureStrategy(ai) {}
        string getName() override { return "cure enhancement boss"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanTotemsStrategy : public ShamanTotemsStrategy
    {
    public:
        EnhancementShamanTotemsStrategy(PlayerbotAI* ai) : ShamanTotemsStrategy(ai) {}

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanTotemsPveStrategy : public EnhancementShamanTotemsStrategy
    {
    public:
        EnhancementShamanTotemsPveStrategy(PlayerbotAI* ai) : EnhancementShamanTotemsStrategy(ai) {}
        string getName() override { return "totems enhancement pve"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanTotemsPvpStrategy : public EnhancementShamanTotemsStrategy
    {
    public:
        EnhancementShamanTotemsPvpStrategy(PlayerbotAI* ai) : EnhancementShamanTotemsStrategy(ai) {}
        string getName() override { return "totems enhancement pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanTotemsBossStrategy : public EnhancementShamanTotemsStrategy
    {
    public:
        EnhancementShamanTotemsBossStrategy(PlayerbotAI* ai) : EnhancementShamanTotemsStrategy(ai) {}
        string getName() override { return "totems enhancement boss"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
