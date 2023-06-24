#pragma once
#include "GenericActions.h"

namespace ai
{
    class BuyAction : public ChatCommandAction
    {
    public:
        BuyAction(PlayerbotAI* ai, string name = "buy") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;

    private:
        bool BuyItem(Player* requester, VendorItemData const* tItems, ObjectGuid vendorguid, const ItemPrototype* proto);

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

    class BuyBackAction : public BuyAction
    {
    public:
        BuyBackAction(PlayerbotAI* ai, string name = "buyback") : BuyAction(ai, name) {}
        virtual bool Execute(Event& event) override;

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "buyback"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This command will make bots retrieve items after being sold.\n"
                "Usage: buyback [itemlink]\n"
                "Example: buyback all (try to get all items back)\n"
                "Example: buyback [itemlink] (get a specific item back)\n";
        }
        virtual vector<string> GetUsedActions() { return { "" }; }
        virtual vector<string> GetUsedValues() { return { "nearest npcs", "item count", "bag space"}; }
#endif 
    };
}
