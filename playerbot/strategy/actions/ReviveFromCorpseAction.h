#pragma once
#include "MovementActions.h"

namespace ai
{
	class ReviveFromCorpseAction : public MovementAction {
	public:
		ReviveFromCorpseAction(PlayerbotAI* ai) : MovementAction(ai, "revive from corpse") {}

    public:
        virtual bool Execute(Event event);
    };

    class FindCorpseAction : public MovementAction {
    public:
        FindCorpseAction(PlayerbotAI* ai) : MovementAction(ai, "find corpse") {}

    public:
        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

	class SpiritHealerAction : public MovementAction {
	public:
	    SpiritHealerAction(PlayerbotAI* ai, string name = "spirit healer") : MovementAction(ai,name) {}

    public:
        virtual WorldSafeLocsEntry const* GetGrave(bool startZone);
        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

}
