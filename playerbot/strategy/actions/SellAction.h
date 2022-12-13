#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class SellAction : public InventoryAction {
    public:
        SellAction(PlayerbotAI* ai, string name = "sell") : InventoryAction(ai, name) {}
        virtual bool Execute(Event& event);

        void Sell(FindItemVisitor* visitor);
        void Sell(Item* item);
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "sell"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This command will make bots sell items to a nearby vendor.\n"
                "Usage: sell qualifier or itemlinks\n"
                "Example: sell vendor (sell based on item use)\n"
                "Example: sell gray\n"
                "Example: sell [itemlink] [itemlink]\n"
                "Example: sell epic \n";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return { "nearest npcs", "item usage" }; }
#endif 
    };
}