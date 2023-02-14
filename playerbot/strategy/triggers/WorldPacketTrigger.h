#pragma once

#include "../Trigger.h"

namespace ai
{
    class WorldPacketTrigger : public Trigger {
    public:
        WorldPacketTrigger(PlayerbotAI* ai, string command) : Trigger(ai, command), triggered(false) {}

        virtual void ExternalEvent(WorldPacket &packet, Player* owner = NULL)
        {
            this->packet = packet;
            this->owner = owner;
            triggered = true;
        }

        virtual Event Check()
        {
            if (!triggered)
                return Event();

            return Event(getName(), packet, owner);
        }

        virtual void Reset()
        {
            triggered = false;
        }

        bool isEqual(WorldPacket m_packet) { string a, b; packet >> a; m_packet >> b;  return a == b; }

    private:
        WorldPacket packet;
        bool triggered;
        Player* owner;
    };
}
