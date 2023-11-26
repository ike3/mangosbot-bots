#pragma once
#include "GenericActions.h"

namespace ai
{
    class TellLosAction : public ChatCommandAction
    {
    public:
        TellLosAction(PlayerbotAI* ai) : ChatCommandAction(ai, "los") {}
        virtual bool Execute(Event& event) override;

    private:
        void ListUnits(Player* requester, string title, list<ObjectGuid> units);
        void ListGameObjects(Player* requester, string title, list<ObjectGuid> gos);
    };
}
