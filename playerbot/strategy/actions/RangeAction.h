#pragma once
#include "GenericActions.h"

namespace ai
{
    class RangeAction : public ChatCommandAction
    {
    public:
        RangeAction(PlayerbotAI* ai) : ChatCommandAction(ai, "range") {}
        virtual bool Execute(Event& event) override;

    private:
        void PrintRange(string type, Player* requester);
    };
}
