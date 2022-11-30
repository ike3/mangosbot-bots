#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    class NearestGameObjects : public ObjectGuidListCalculatedValue, public Qualified
	{
	public:
        NearestGameObjects(PlayerbotAI* ai, float range = sPlayerbotAIConfig.sightDistance, bool ignoreLos = false, string name = "nearest game objects") :
            ObjectGuidListCalculatedValue(ai, name), range(range) , ignoreLos(ignoreLos) {}

    protected:
        virtual list<ObjectGuid> Calculate();

    private:
        float range;
        bool ignoreLos;
	};

    class NearestDynamicObjects : public ObjectGuidListCalculatedValue
    {
    public:
        NearestDynamicObjects(PlayerbotAI* ai, float range = sPlayerbotAIConfig.farDistance, bool ignoreLos = false, string name = "nearest dynamic objects") :
            ObjectGuidListCalculatedValue(ai, name), range(range), ignoreLos(ignoreLos) {}

    protected:
        virtual list<ObjectGuid> Calculate();

    private:
        float range;
        bool ignoreLos;
    };
}
