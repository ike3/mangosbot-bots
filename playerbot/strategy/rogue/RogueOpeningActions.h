#pragma once

namespace ai
{
	class CastSapAction : public CastMeleeSpellAction
	{ 
	public: 
		CastSapAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "sap") {} 
        virtual Value<Unit*>* GetTargetValue()
        {
            return context->GetValue<Unit*>("cc target", getName());
        }
        virtual bool isUseful() { return true; }
	};

	class CastGarroteAction : public CastMeleeSpellAction 
	{ 
	public: 
		CastGarroteAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "garrote") {} 
	};

	class CastCheapShotAction : public CastMeleeSpellAction 
	{ 
	public: 
		CastCheapShotAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "cheap shot") {} 
	};

    class CastAmbushAction : public CastMeleeSpellAction
    {
    public:
        CastAmbushAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "ambush") {}
    };
}
