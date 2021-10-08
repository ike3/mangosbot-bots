#pragma once
#include "../Value.h"

namespace ai
{
    class FindPlayerPredicate
    {
    public:
        virtual bool Check(Unit*) = 0;
    };

    class SpellEntryPredicate
    {
    public:
        virtual bool Check(SpellEntry const*) = 0;
    };

    class PartyMemberValue : public UnitCalculatedValue
	{
	public:
        PartyMemberValue(PlayerbotAI* ai, string name = "party member") : UnitCalculatedValue(ai, name) {}

    public:
        bool IsTargetOfSpellCast(Player* target, SpellEntryPredicate &predicate);

    protected:
        Unit* FindPartyMember(FindPlayerPredicate &predicate, bool ignoreOutOfGroup = false);
        Unit* FindPartyMember(list<Player*>* party, FindPlayerPredicate &predicate);
        bool Check(Unit* player);
	};
}
