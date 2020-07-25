#include "botpch.h"
#include "../../playerbot.h"
#include "RangeValues.h"

using namespace ai;

RangeValue::RangeValue(PlayerbotAI* ai)
    : ManualSetValue<float>(ai, 0, "range"), Qualified()
{
}
