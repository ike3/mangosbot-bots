#pragma once

#include "../Action.h"
#include "QueryItemUsageAction.h"
#include "LootAction.h"

namespace ai
{
    class LootRollAction : public QueryItemUsageAction {
    public:
        LootRollAction(PlayerbotAI* ai, string name = "loot roll") : QueryItemUsageAction(ai, name) {}
        virtual bool Execute(Event event);

    protected:
        virtual RollVote CalculateRollVote(ItemPrototype const *proto);
    };

    class MasterLootRollAction : public LootRollAction {
    public:
        MasterLootRollAction(PlayerbotAI* ai) : LootRollAction(ai, "master loot roll") {}
        virtual bool isUseful() { return !ai->HasActivePlayerMaster(); };
        virtual bool Execute(Event event);
    };
}
