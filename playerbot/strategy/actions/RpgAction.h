#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class RpgAction : public MovementAction {
    public:
        RpgAction(PlayerbotAI* ai) : MovementAction(ai, "rpg") {}

        virtual bool Execute(Event event);
        virtual bool isUseful();

    private:
        typedef void (RpgAction::*RpgElement) (Unit*);

        void talk(Unit* unit) { emote(unit, EMOTE_ONESHOT_TALK); }
        void beg(Unit* unit) { emote(unit, EMOTE_ONESHOT_KNEEL); }
        void rude(Unit* unit) { emote(unit, EMOTE_ONESHOT_RUDE); }
        void cry(Unit* unit) { emote(unit, EMOTE_ONESHOT_CRY); }
        void point(Unit* unit) { emote(unit, EMOTE_ONESHOT_POINT); }

        void dance(Unit* unit) { longemote(unit, EMOTE_STATE_DANCE); }
        void work(Unit* unit) { longemote(unit, EMOTE_STATE_WORK); }

        void stay(Unit* unit);
        void cancel(Unit* unit);

        void emote(Unit* unit, uint32 type);
        void longemote(Unit* unit, uint32 type);

    };

}
