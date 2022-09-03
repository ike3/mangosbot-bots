#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class AhAction : public InventoryAction
    {
    public:
        AhAction(PlayerbotAI* ai, string name = "ah") : InventoryAction(ai, name) {}
        virtual bool Execute(Event event);        

    private:
        virtual bool Execute(string text, Unit* auctioneer);
        bool PostItem(Item* item, uint32 price, Unit* auctioneer, uint32 time);
    };


    class AhBidAction : public AhAction
    {
    public:
        AhBidAction(PlayerbotAI* ai) : AhAction(ai, "ah bid") {}

    private:
        virtual bool Execute(string text, Unit* auctioneer);
        bool BidItem(AuctionEntry* auction, uint32 price, Unit* auctioneer);
    };
}
