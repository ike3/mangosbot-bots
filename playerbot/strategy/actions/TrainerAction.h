#pragma once
#include "GenericActions.h"

namespace ai
{
	class TrainerAction : public ChatCommandAction
    {
	public:
		TrainerAction(PlayerbotAI* ai) : ChatCommandAction(ai, "trainer") {}
        virtual bool Execute(Event& event) override;

    private:
        typedef void (TrainerAction::*TrainerSpellAction)(uint32, TrainerSpell const*, ostringstream& msg);
        void Iterate(Creature* creature, TrainerSpellAction action, SpellIds& spells);
        void Learn(uint32 cost, TrainerSpell const* tSpell, ostringstream& msg);
        void TellHeader(Creature* creature);
        void TellFooter(uint32 totalCost);
    };
}