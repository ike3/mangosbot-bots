#pragma once
#include "GenericActions.h"

namespace ai
{
    class ListSpellsAction : public ChatCommandAction
    {
    public:
        ListSpellsAction(PlayerbotAI* ai, string name = "spells") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
        virtual list<pair<uint32, string> > GetSpellList(string filter = "");

    private:
        static map<uint32, SkillLineAbilityEntry const*> skillSpells;
        static set<uint32> vendorItems;
    };
}
