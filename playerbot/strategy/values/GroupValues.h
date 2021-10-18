#pragma once
#include "../Value.h"

namespace ai
{
    class GroupMembersValue : public ObjectGuidListCalculatedValue
    {
    public:
        GroupMembersValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "group members",2) {}

        virtual list<ObjectGuid> Calculate();
    };

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

    class GroupBoolCountValue : public Uint32CalculatedValue, public Qualified
    {
    public:
        GroupBoolCountValue(PlayerbotAI* ai) : Uint32CalculatedValue(ai, "group count") {}

        virtual uint32 Calculate();
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

    class GroupReadyValue : public BoolCalculatedValue, public Qualified
    {
    public:
        GroupReadyValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "group ready", 2) {}

        virtual bool Calculate();
    };
}
