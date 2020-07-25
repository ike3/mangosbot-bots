#pragma once
#include "../Value.h"

namespace ai
{
    class AlwaysLootListValue : public ManualSetValue<set<uint32>&>
	{
	public:
        AlwaysLootListValue(PlayerbotAI* ai) : ManualSetValue<set<uint32>&>(ai, list) {}

        virtual string Save();
        virtual bool Load(string value);

    private:
        set<uint32> list;
    };
}
