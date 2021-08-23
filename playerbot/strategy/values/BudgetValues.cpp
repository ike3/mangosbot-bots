#include "botpch.h"
#include "../../playerbot.h"
#include "BudgetValues.h"

using namespace ai;

uint32 MoneyNeededValue::Calculate()
{
	PlayerbotAI* ai = bot->GetPlayerbotAI();
	AiObjectContext* context = ai->GetAiObjectContext();

	uint32 level = bot->getLevel();

	uint32 moneyWanted = 1000; //We want atleast 10 silver.

	moneyWanted = std::max(moneyWanted, AI_VALUE(uint32, "repair cost") * 2); //Or twice the current repair cost.

	moneyWanted = std::max(moneyWanted, level * level * level); //Or level^2 (10s @ lvl10, 3g @ lvl30, 20g @ lvl60, 50g @ lvl80)

	return moneyWanted;
};