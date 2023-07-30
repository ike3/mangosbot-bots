#pragma once

#include "../Trigger.h"

namespace ai
{
    class ChatCommandTrigger : public Trigger {
    public:
        ChatCommandTrigger(PlayerbotAI* ai, string command) : Trigger(ai, command, 1), triggered(false), owner(NULL) {}

        virtual void ExternalEvent(string param, Player* owner = NULL)
        {
            this->param = param;
            this->owner = owner;
            triggered = true;
        }

        virtual Event Check()
        {
            if (!triggered)
                return Event();

            return Event(getName(), param, owner);
        }

        virtual void Reset()
        {
            triggered = false;
        }

   private:
        string param;
        bool triggered;
        Player* owner;
    };
}
