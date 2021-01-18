#pragma once

#include "../Value.h"

namespace ai
{
class LfgProposalValue : public ManualSetValue<uint32>
{
public:
    LfgProposalValue(PlayerbotAI* ai) : ManualSetValue<uint32>(ai, 0, "lfg proposal") {}
};

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
}
