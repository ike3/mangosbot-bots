#pragma once
#include "../Strategy.h"

namespace ai
{
    class HunterBuffSpeedStrategy : public Strategy
    {
    public:
        HunterBuffSpeedStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bspeed"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class HunterBuffManaStrategy : public Strategy
    {
    public:
        HunterBuffManaStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bmana"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

	class HunterBuffDpsStrategy : public Strategy
	{
	public:
		HunterBuffDpsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bdps"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

	class HunterNatureResistanceStrategy : public Strategy
	{
	public:
		HunterNatureResistanceStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "rnature"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};
}
