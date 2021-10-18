#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{

    class ItemForSpellValue : public CalculatedValue<Item*>, public Qualified
	{
	public:
        ItemForSpellValue(PlayerbotAI* ai, string name = "item for spell") : CalculatedValue<Item*>(ai, name) {}

    public:
        virtual Item* Calculate();

    private:
        Item* GetItemFitsToSpellRequirements(uint8 slot, SpellEntry const *spellInfo);

    };
}
