#pragma once
#include "GenericActions.h"

namespace ai
{
    class StatsAction : public ChatCommandAction
    {
    public:
        StatsAction(PlayerbotAI* ai) : ChatCommandAction(ai, "stats") {}
        virtual bool Execute(Event& event) override;

    private:
        void ListBagSlots(ostringstream &out);
        void ListXP(ostringstream &out);
        void ListRepairCost(ostringstream &out);
        void ListGold(ostringstream &out);
        void ListPower(ostringstream& out);
        uint32 EstRepair(uint16 pos);
        double RepairPercent(uint16 pos);
    };
}
