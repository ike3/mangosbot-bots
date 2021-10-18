#pragma once
#include "../Value.h"

namespace ai
{
    class SkipSpellsListValue : public ManualSetValue<set<uint32>&>
	{
	public:
        SkipSpellsListValue(PlayerbotAI* ai, string name = "skip spells list") : ManualSetValue<set<uint32>&>(ai, list, name) {}

        virtual string Save();
        virtual bool Load(string value);

    private:
        set<uint32> list;
    };
}
