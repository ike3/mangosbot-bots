#pragma once
#include "../Value.h"

namespace ai
{
    class MyThreatValue : public LogCalculatedValue<float>, public Qualified
    {
    public:
        MyThreatValue(PlayerbotAI* ai, string name = "my threat") : LogCalculatedValue(ai, name) { minChangeInterval = 1; }
        virtual bool EqualToLast(float value) { return value == lastValue; }
    public:
        virtual float Calculate();
    };

    class TankThreatValue : public FloatCalculatedValue, public Qualified
    {
    public:
        TankThreatValue(PlayerbotAI* ai, string name = "tank threat") : FloatCalculatedValue(ai, name) {}

    public:
        virtual float Calculate();
    };

    class ThreatValue : public Uint8CalculatedValue, public Qualified
	{
	public:
        ThreatValue(PlayerbotAI* ai, string name = "threat") : Uint8CalculatedValue(ai, name) {}

    public:
    	virtual uint8 Calculate();

        static float GetThreat(Player* player, Unit* target);
        static float GetTankThreat(PlayerbotAI* ai, Unit* target);
    protected:
    	uint8 Calculate(Unit* target);
    };

    /* Abanoded attempt
    class SpellThreatValue : public FloatCalculatedValue, public Qualified
    {
    public:
        SpellThreatValue(PlayerbotAI* ai, string name = "spell threat") : FloatCalculatedValue(ai, name) {}

    public:
        virtual float Calculate();
    protected:
        bool IsEffectWithImplementedMultiplier(uint32 effectId) const;
        int32 SpellDamage(SpellEntry const* spellInfo)
    };
    */
}
