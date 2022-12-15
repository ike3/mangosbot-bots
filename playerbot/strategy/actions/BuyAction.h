#pragma once
#include "GenericActions.h"

namespace ai
{
    class BuyAction : public ChatCommandAction
    {
    public:
        BuyAction(PlayerbotAI* ai) : ChatCommandAction(ai, "buy") {}
        virtual bool Execute(Event& event) override;

    private:
        bool BuyItem(VendorItemData const* tItems, ObjectGuid vendorguid, const ItemPrototype* proto);
        bool TradeItem(FindItemVisitor *visitor, int8 slot);
        bool TradeItem(const Item& item, int8 slot);
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "buy"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This command will make bots buy items from a nearby vendor.\n"
                "Usage: buy [itemlink]\n"
                "Example: buy usefull (buy based on item use)\n"
                "Example: buy [itemlink]\n";
        }
        virtual vector<string> GetUsedActions() { return { "equip upgrades" }; }
        virtual vector<string> GetUsedValues() { return { "nearest npcs", "item count", "item usage", "free money for"}; }
#endif 
    };
}
