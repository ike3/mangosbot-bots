#pragma once
#include "../Value.h"

namespace ai
{
    class SeeSpellLocationValue : public LogCalculatedValue<WorldPosition>
    {
    public:
        SeeSpellLocationValue(PlayerbotAI* ai, string name = "see spell location") : LogCalculatedValue(ai, name) {};

        virtual bool EqualToLast(WorldPosition value) { return value == lastValue; };

        WorldPosition Calculate() { return value; }
    };

    class RTSCSelectedValue : public ManualSetValue<bool>
	{
	public:
        RTSCSelectedValue(PlayerbotAI* ai, bool defaultvalue = false, string name = "RTSC selected") : ManualSetValue(ai, defaultvalue,name) {};
    };

    class RTSCNextSpellActionValue : public ManualSetValue<string>
    {
    public:
        RTSCNextSpellActionValue(PlayerbotAI* ai, string defaultvalue = "", string name = "RTSC next spell action") : ManualSetValue(ai, defaultvalue, name) {};
    };

    class RTSCSavedLocationValue : public ManualSetValue<WorldPosition>, public Qualified
    {
    public:
        RTSCSavedLocationValue(PlayerbotAI* ai, WorldPosition defaultvalue = WorldPosition(), string name = "RTSC saved location") : ManualSetValue(ai, defaultvalue, name) {};
        virtual string Save() { return value.to_string(); }
        virtual bool Load(string text) { value = WorldPosition(text); return true; }
    };
}
