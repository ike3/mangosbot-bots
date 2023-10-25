#pragma once
#include "../Action.h"
#include "../../PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"

namespace ai
{
    class UpdateGearAction : public Action
    {
    public:
        UpdateGearAction(PlayerbotAI* ai);
        virtual bool Execute(Event& event);
        virtual bool isUseful();

    private:
        uint8 GetProgressionLevel(uint32 itemLevel);
        uint8 GetMasterAverageProgressionLevel();
        uint8 GetMasterItemProgressionLevel(uint8 slot, uint8 avgProgressionLevel);
       
        bool CanEquipUnseenItem(uint8 slot, uint16& dest, uint32 itemId);
        void EnchantItem(Item* item);

    private:
        std::vector<EnchantTemplate> enchants;
    };
}
