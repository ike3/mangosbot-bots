#pragma once
#include "DungeonActions.h"

namespace ai
{
    class OnyxiasLairEnableDungeonStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        OnyxiasLairEnableDungeonStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "enable onyxia's lair strategy", "onyxia's lair", true) {}
    };

    class OnyxiasLairDisableDungeonStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        OnyxiasLairDisableDungeonStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "disable onyxia's lair strategy", "onyxia's lair", false) {}
    };

    class OnyxiaEnableFightStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        OnyxiaEnableFightStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "enable onyxia fight strategy", "onyxia", true) {}
    };

    class OnyxiaDisableFightStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        OnyxiaDisableFightStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "disable onyxia fight strategy", "onyxia", false) {}
    };
}
