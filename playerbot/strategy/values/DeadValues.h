#pragma once
#include "../Value.h"

namespace ai
{
    class GraveyardValue : public GuidPositionCalculatedValue, public Qualified
    {
    public:
        GraveyardValue(PlayerbotAI* ai, string name = "graveyard", int checkInterval = 10) : GuidPositionCalculatedValue(ai, name, checkInterval), Qualified() {}

    public:
        GuidPosition Calculate();
    };

    class BestGraveyardValue : public GuidPositionCalculatedValue
    {
    public:
        BestGraveyardValue(PlayerbotAI* ai, string name = "best graveyard", int checkInterval = 10) : GuidPositionCalculatedValue(ai, name, checkInterval) {}

    public:
        GuidPosition Calculate();
    };

    class ShouldSpiritHealerValue : public BoolCalculatedValue
    {
    public:
        ShouldSpiritHealerValue(PlayerbotAI* ai, string name = "should spirit healer") : BoolCalculatedValue(ai, name) {}

    public:
        bool Calculate();
    };
}
