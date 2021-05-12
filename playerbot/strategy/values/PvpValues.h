#pragma once

#include "../Value.h"

namespace ai
{
class BgTypeValue : public ManualSetValue<uint32>
{
public:
    BgTypeValue(PlayerbotAI* ai) : ManualSetValue<uint32>(ai, 0, "bg type") {}
};

class ArenaTypeValue : public ManualSetValue<uint32>
{
public:
    ArenaTypeValue(PlayerbotAI* ai) : ManualSetValue<uint32>(ai, 0, "arena type") {}
};

class BgRoleValue : public ManualSetValue<uint32>
{
public:
    BgRoleValue(PlayerbotAI* ai) : ManualSetValue<uint32>(ai, 0, "bg role") {}
};

class BgMasterValue : public ManualSetValue<ObjectGuid>
{
public:
    BgMasterValue(PlayerbotAI* ai) : ManualSetValue<ObjectGuid>(ai, ObjectGuid()) {}
};

class FlagCarrierValue : public UnitCalculatedValue
{
public:
    FlagCarrierValue(PlayerbotAI* ai, bool sameTeam = false, bool ignoreRange = false) :
        UnitCalculatedValue(ai), sameTeam(sameTeam), ignoreRange(ignoreRange) {}

    virtual Unit* Calculate();

private:
    bool sameTeam;
    bool ignoreRange;
};
}
