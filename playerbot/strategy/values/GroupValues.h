#pragma once
#include "../Value.h"

namespace ai
{
    class IsFollowingPartyValue : public BoolCalculatedValue
    {
    public:
        IsFollowingPartyValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "following party") {}
        
        virtual bool Calculate();
    };

    class IsNearLeaderValue : public BoolCalculatedValue
    {
    public:
        IsNearLeaderValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "near leader") {}

        virtual bool Calculate();
    };

    class BoolANDValue : public BoolCalculatedValue, public Qualified
    {
    public:
        BoolANDValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "bool and") {}

        virtual bool Calculate();
    };
    
    class GroupBoolANDValue : public BoolCalculatedValue, public Qualified
    {
    public:
        GroupBoolANDValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "group bool and") {}

        virtual bool Calculate();
    };

    class GroupBoolORValue : public BoolCalculatedValue, public Qualified
    {
    public:
        GroupBoolORValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "group bool or") {}

        virtual bool Calculate();
    };
}
