#pragma once

#include "../../ServerFacade.h"
#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
	class ReleaseSpiritAction : public Action {
	public:
		ReleaseSpiritAction(PlayerbotAI* ai, string name = "release") : Action(ai, name) {}

    public:
        virtual bool Execute(Event event)
        {
            if (sServerFacade.IsAlive(bot) || bot->GetCorpse())
            {
                ai->TellError("I am not dead");
                return false;
            }

            ai->ChangeStrategy("-follow,+stay", BOT_STATE_NON_COMBAT);

            bot->SetBotDeathTimer();
            bot->BuildPlayerRepop();

            bot->RepopAtGraveyard();
            ai->TellMaster("Meet me at the graveyard");
            return true;
        }
    };

    class AutoReleaseSpiritAction : public ReleaseSpiritAction {
    public:
        AutoReleaseSpiritAction(PlayerbotAI* ai, string name = "auto release") : ReleaseSpiritAction(ai, name) {}

        virtual bool Execute(Event event)
        {
            bot->SetBotDeathTimer();
            bot->BuildPlayerRepop();

            bot->RepopAtGraveyard();
            return true;
        }

        virtual bool isUseful() { return !bot->GetGroup() && !sServerFacade.IsAlive(bot) && !bot->GetCorpse(); }
    };
}
