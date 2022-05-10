#pragma once

#include "../Value.h"

namespace ai
{
class LfgProposalValue : public ManualSetValue<uint32>
{
public:
    LfgProposalValue(PlayerbotAI* ai) : ManualSetValue<uint32>(ai, 0, "lfg proposal") {}
};

class BotRolesValue : public Uint8CalculatedValue, public Qualified
{
public:
    BotRolesValue(PlayerbotAI* ai, string name = "bot roles") : Uint8CalculatedValue(ai, name, 10) {}
    virtual uint8 Calculate()
    {
        return AiFactory::GetPlayerRoles(bot);
    }
};
}
