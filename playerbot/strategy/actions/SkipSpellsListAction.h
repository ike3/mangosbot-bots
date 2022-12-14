#pragma once
#include "../../LootObjectStack.h"
#include "GenericActions.h"

namespace ai
{
    class SkipSpellsListAction : public ChatCommandAction
    {
    public:
        SkipSpellsListAction(PlayerbotAI* ai) : ChatCommandAction(ai, "ss") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        SpellIds parseIds(string text);
    };
}
