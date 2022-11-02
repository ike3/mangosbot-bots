#pragma once

#include "../Action.h"
#include "UseItemAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class DrinkAction : public UseItemAction
    {
    public:
        DrinkAction(PlayerbotAI* ai) : UseItemAction(ai, "drink") {}

        virtual bool Execute(Event event)
        {
            if (sServerFacade.IsInCombat(bot))
                return false;

            if (!bot->HasMana())
                return false;

            if (sPlayerbotAIConfig.freeFood)
            {
                if (bot->IsNonMeleeSpellCasted(true))
                    return false;

                bot->clearUnitState(UNIT_STAT_CHASE);
                bot->clearUnitState(UNIT_STAT_FOLLOW);

                if (sServerFacade.isMoving(bot))
                {
                    ai->StopMoving();
                    ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
                    return false;
                }

                bot->addUnitState(UNIT_STAND_STATE_SIT);
                ai->InterruptSpell();

                //float hp = bot->GetHealthPercent();
                float mp = bot->HasMana() ? bot->GetPowerPercent() : 0.f;
                float p = mp;
                float delay;

                if (!bot->InBattleGround())
                    delay = 27000.0f * (100 - p) / 100.0f;
                else
                    delay = 20000.0f * (100 - p) / 100.0f;

                const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(24355);
                if (!pSpellInfo)
                    return false;

                if (bot->IsMounted())
                {
                    WorldPacket emptyPacket;
                    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
                }

                ai->CastSpell(24355, bot);
                if (AI_VALUE2(uint8, "health", "self target") < sPlayerbotAIConfig.mediumHealth)
                    bot->CastSpell(bot, 24005, TRIGGERED_OLD_TRIGGERED);

                ai->SetNextCheckDelay(delay);
                bot->RemoveSpellCooldown(*pSpellInfo);

                return true;
            }

            return UseItemAction::Execute(event);
        }

        virtual bool isUseful()
        {
            return UseItemAction::isUseful() && AI_VALUE2(uint8, "mana", "self target") < 85;
        }

        virtual bool isPossible()
        {
            return sPlayerbotAIConfig.freeFood || UseItemAction::isPossible();
        }
    };

    class EatAction : public UseItemAction
    {
    public:
        EatAction(PlayerbotAI* ai) : UseItemAction(ai, "food") {}

        virtual bool Execute(Event event)
        {
            if (sServerFacade.IsInCombat(bot))
                return false;

            if (sPlayerbotAIConfig.freeFood)
            {
                if (bot->IsNonMeleeSpellCasted(true))
                    return false;

                bot->clearUnitState(UNIT_STAT_CHASE);
                bot->clearUnitState(UNIT_STAT_FOLLOW);

                if (sServerFacade.isMoving(bot))
                {
                    ai->StopMoving();
                    ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
                    return false;
                }

                bot->addUnitState(UNIT_STAND_STATE_SIT);
                ai->InterruptSpell();

                float hp = bot->GetHealthPercent();
                //float mp = bot->HasMana() ? bot->GetPowerPercent() : 0.f;
                float p = hp;
                float delay;

                if (!bot->InBattleGround())
                    delay = 27000.0f * (100 - p) / 100.0f;
                else
                    delay = 20000.0f * (100 - p) / 100.0f;

                const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(24005);
                if (!pSpellInfo)
                    return false;

                if (bot->IsMounted())
                {
                    WorldPacket emptyPacket;
                    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
                }

                ai->CastSpell(24005, bot);
                if (AI_VALUE2(uint8, "mana", "self target") < 85)
                    bot->CastSpell(bot, 24355, TRIGGERED_OLD_TRIGGERED);

                ai->SetNextCheckDelay(delay);
                bot->RemoveSpellCooldown(*pSpellInfo);

                return true;
            }

            return UseItemAction::Execute(event);
        }

        virtual bool isUseful()
        {
            return UseItemAction::isUseful() && AI_VALUE2(uint8, "health", "self target") < sPlayerbotAIConfig.lowHealth;
        }

        virtual bool isPossible()
        {
            return sPlayerbotAIConfig.freeFood || UseItemAction::isPossible();
        }
    };

}
