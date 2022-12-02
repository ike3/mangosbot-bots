#pragma once
#include "../Strategy.h"

namespace ai
{
    class PaladinBuffWisdomStrategy : public Strategy
    {
    public:
		PaladinBuffWisdomStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bwisdom"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
		void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class PaladinBuffMightStrategy : public Strategy
    {
    public:
		PaladinBuffMightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bmight"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

	class PaladinBuffArmorStrategy : public Strategy
	{
	public:
		PaladinBuffArmorStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "barmor"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

	class PaladinBuffConcentrationStrategy : public Strategy
	{
	public:
		PaladinBuffConcentrationStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bconcentration"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

	class PaladinBuffSanctityStrategy : public Strategy
	{
	public:
		PaladinBuffSanctityStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bsanctity"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

    class PaladinBuffAoeStrategy : public Strategy
    {
    public:
        PaladinBuffAoeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "baoe"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

	class PaladinBuffThreatStrategy : public Strategy
	{
	public:
		PaladinBuffThreatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bthreat"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

	class PaladinBuffSpeedStrategy : public Strategy
	{
	public:
		PaladinBuffSpeedStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "crusader"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

    class PaladinBuffKingsStrategy : public Strategy
    {
    public:
		PaladinBuffKingsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "bkings"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

	class PaladinShadowResistanceStrategy : public Strategy
	{
	public:
		PaladinShadowResistanceStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "rshadow"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

	class PaladinFrostResistanceStrategy : public Strategy
	{
	public:
		PaladinFrostResistanceStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "rfrost"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};

	class PaladinFireResistanceStrategy : public Strategy
	{
	public:
		PaladinFireResistanceStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		string getName() override { return "rfire"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
	};
}
