#pragma once
#include "../Value.h"

namespace ai
{
    typedef list<string> Outfit;
    class OutfitListValue : public ManualSetValue<Outfit&>
	{
	public:
        OutfitListValue(PlayerbotAI* ai, string name = "outfit list") : ManualSetValue<Outfit&>(ai, list, name) {}

        virtual string Save();
        virtual bool Load(string value);

    private:
        Outfit list;
    };
}
