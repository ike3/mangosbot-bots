#pragma once
#include "DungeonActions.h"

namespace ai
{
    class MoltenCoreEnableDungeonStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        MoltenCoreEnableDungeonStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "enable molten core strategy", "molten core", true) {}
    };

    class MoltenCoreDisableDungeonStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        MoltenCoreDisableDungeonStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "disable molten core strategy", "molten core", false) {}
    };

    class MagmadarEnableFightStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        MagmadarEnableFightStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "enable magmadar fight strategy", "magmadar", true) {}
    };

    class MagmadarDisableFightStrategyAction : public ChangeDungeonStrategyAction
    {
    public:
        MagmadarDisableFightStrategyAction(PlayerbotAI* ai) : ChangeDungeonStrategyAction(ai, "disable magmadar fight strategy", "magmadar", false) {}
    };

    class MagmadarMoveAwayFromLavaBombAction : public MoveAwayFromGameObject
    {
    public:
        MagmadarMoveAwayFromLavaBombAction(PlayerbotAI* ai) : MoveAwayFromGameObject(ai, "move away from magmadar lava bomb", 177704, 2.5f) {}
    };
}