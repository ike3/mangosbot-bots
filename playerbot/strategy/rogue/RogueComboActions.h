#pragma once

namespace ai
{
	class CastComboAction : public CastMeleeSpellAction
	{
	public:
	    CastComboAction(PlayerbotAI* ai, string name) : CastMeleeSpellAction(ai, name) {}

	    virtual bool isUseful()
	    {
	        return CastMeleeSpellAction::isUseful() && AI_VALUE2(uint8, "combo", "current target") < 5;
	    }
	};

	class CastSinisterStrikeAction : public CastComboAction
	{
	public:
		CastSinisterStrikeAction(PlayerbotAI* ai) : CastComboAction(ai, "sinister strike") {}
	};

    class CastMutilateAction : public CastComboAction
    {
    public:
        CastMutilateAction(PlayerbotAI* ai) : CastComboAction(ai, "mutilate") {}
    };

    class CastRiposteAction : public CastComboAction
    {
    public:
        CastRiposteAction(PlayerbotAI* ai) : CastComboAction(ai, "riposte") {}
    };

	class CastGougeAction : public CastComboAction
	{
	public:
		CastGougeAction(PlayerbotAI* ai) : CastComboAction(ai, "gouge") {}
	};

    class CastBackstabAction : public CastComboAction
    {
    public:
        CastBackstabAction(PlayerbotAI* ai) : CastComboAction(ai, "backstab") {}
    };

	class CastHemorrhageAction : public CastComboAction
	{
	public:
		CastHemorrhageAction(PlayerbotAI* ai) : CastComboAction(ai, "hemorrhage") {}
	};

	class CastGhostlyStrikeAction : public CastComboAction
	{
	public:
		CastGhostlyStrikeAction(PlayerbotAI* ai) : CastComboAction(ai, "ghostly strike") {}
		virtual bool isUseful()
		{
			return CastComboAction::isUseful() && GetTarget() && (GetTarget()->getClass() == CLASS_WARRIOR || GetTarget()->getClass() == CLASS_ROGUE);
		}
	};

	SPELL_ACTION(CastPremeditationAction, "premeditation");
}
