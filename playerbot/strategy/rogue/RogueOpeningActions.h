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
        /*virtual bool isPossible()
        {
            return ai->CanCastSpell("sap", GetTarget(), true);
        }*/
        virtual bool isUseful() { return true; }
        /*virtual NextAction** getPrerequisites() {
            return NextAction::merge(NextAction::array(0, new NextAction("stealth"), NULL), CastSpellAction::getPrerequisites());
        }*/
        /*virtual bool Execute(Event event) {
            Unit* sapTarget = context->GetValue<Unit*>("cc target", getName())->Get();
            if (sapTarget && !ai->HasAura("sap", sapTarget))
            {
                ObjectGuid guid = sapTarget->GetObjectGuid();
                bot->SetSelectionGuid(sapTarget->GetObjectGuid());
                Unit* oldTarget = context->GetValue<Unit*>("current target")->Get();
                context->GetValue<Unit*>("old target")->Set(oldTarget);
                context->GetValue<Unit*>("current target")->Set(sapTarget);
                ai->CastSpell("sap", sapTarget);
                //ai->ChangeEngine(BOT_STATE_COMBAT);
            }
            return true;
        }*/
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
