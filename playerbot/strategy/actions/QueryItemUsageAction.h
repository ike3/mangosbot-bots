#pragma once
#include "GenericActions.h"

namespace ai
{
    class ItemQualifier;

    class QueryItemUsageAction : public ChatCommandAction
    {
    public:
        QueryItemUsageAction(PlayerbotAI* ai, string name = "query item usage") : ChatCommandAction(ai, name) {}

    protected:
        virtual bool Execute(Event& event) override;
        uint32 GetCount(ItemQualifier& qualifier);
        string QueryItem(ItemQualifier& qualifier, uint32 count, uint32 total);
        string QueryItemUsage(ItemQualifier& qualifier);
        string QueryItemPrice(ItemQualifier& qualifier);
        string QueryQuestItem(uint32 itemId, const Quest *questTemplate, const QuestStatusData *questStatus);
        string QueryQuestItem(uint32 itemId);
        string QueryItemPower(ItemQualifier& qualifier);

    private:
        ostringstream out;
    };
}
