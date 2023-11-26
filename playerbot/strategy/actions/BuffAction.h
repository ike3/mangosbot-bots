#pragma once
#include "GenericActions.h"

namespace ai
{
    class BuffAction : public ChatCommandAction
    {
    public:
        BuffAction(PlayerbotAI* ai) : ChatCommandAction(ai, "buff") {}
        virtual bool Execute(Event& event);

    private:
        void TellHeader(uint32 subClass, Player* requester);
    };
}
