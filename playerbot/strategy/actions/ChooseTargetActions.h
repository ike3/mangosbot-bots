#pragma once

#include "../Action.h"
#include "AttackAction.h"
#include "../../ServerFacade.h"
#include "../../playerbot.h"

namespace ai
{
    class DpsAoeAction : public AttackAction
    {
    public:
        DpsAoeAction(PlayerbotAI* ai) : AttackAction(ai, "dps aoe") {}

        virtual string GetTargetName() { return "dps aoe target"; }
    };

    class DpsAssistAction : public AttackAction
    {
    public:
        DpsAssistAction(PlayerbotAI* ai) : AttackAction(ai, "dps assist") {}

        virtual string GetTargetName() { return "dps target"; }
    };

    class TankAssistAction : public AttackAction
    {
    public:
        TankAssistAction(PlayerbotAI* ai) : AttackAction(ai, "tank assist") {}
        virtual string GetTargetName() { return "tank target"; }
    };

    class AttackAnythingAction : public AttackAction
    {
    private:
        bool GrindAlone(Player* bot) //Todo: add specific conditions when bots should always be active (ie. in a guild with a player, some day grouped with a player, ect.)
        {
            if (!sRandomPlayerbotMgr.IsRandomBot(bot))
                return true;

            if (ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get().size() < urand(10,30))
                return true;

            if (sPlayerbotAIConfig.randomBotGrindAlone <= 0)
                return false;

            uint32 randnum = bot->GetGUIDLow();                            //Semi-random but fixed number for each bot.
            uint32 cycle = floor(WorldTimer::getMSTime() / (1000));        //Semi-random number adds 1 each second.

            cycle = cycle * sPlayerbotAIConfig.randomBotGrindAlone / 6000; //Cycles 0.01 per minute for each 1% of the config. (At 100% this is 1 per minute)
            randnum  += cycle;                                     //Random number that increases 0.01 each minute for each % that the bots should be active.
            randnum = (randnum % 100);                                     //Loops the randomnumber at 100. Bassically removes all the numbers above 99. 
            randnum = randnum + 1;                                         //Now we have a number unique for each bot between 1 and 100 that increases by 0.01 (per % active each minute).

            return randnum < sPlayerbotAIConfig.randomBotGrindAlone;       //The given percentage of bots should be active and rotate 1% of those active bots each minute.
        }
    public:
        AttackAnythingAction(PlayerbotAI* ai) : AttackAction(ai, "attack anything") {}
        virtual string GetTargetName() { return "grind target"; }
        virtual bool isUseful() {
            return GetTarget() &&
                /*    (!AI_VALUE(list<ObjectGuid>, "nearest non bot players").empty() &&
                        AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.mediumHealth &&
                        (!AI_VALUE2(uint8, "mana", "self target") || AI_VALUE2(uint8, "mana", "self target") > sPlayerbotAIConfig.mediumMana)
                    ) || AI_VALUE2(bool, "combat", "self target")
    */
                (
                    (
                        (!AI_VALUE(list<ObjectGuid>, "nearest non bot players").empty() || bot->InBattleGround() || GrindAlone(bot))          //Bot is not alone or in battleground or allowed to grind alone.
                        &&
                        AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.mediumHealth                                           //Bot has enough health.
                        &&
                        (!AI_VALUE2(uint8, "mana", "self target") || AI_VALUE2(uint8, "mana", "self target") > sPlayerbotAIConfig.mediumMana) //Bot has no mana or enough mana.
                        &&
                        !context->GetValue<ObjectGuid>("travel target")->Get()                                                                //Bot is not traveling.
                    )
                    ||
                    AI_VALUE2(bool, "combat", "self target")                                                                                  //Bot is already in combat
                )
                ;
        }
        virtual bool isPossible()
        {
            return AttackAction::isPossible() && GetTarget();
        }

        virtual bool Execute(Event event)
        {
            bool result = AttackAction::Execute(event);
            if (result && GetTarget()) context->GetValue<ObjectGuid>("pull target")->Set(GetTarget()->GetObjectGuid());
            return result;
        }
    };

    class AttackLeastHpTargetAction : public AttackAction
    {
    public:
        AttackLeastHpTargetAction(PlayerbotAI* ai) : AttackAction(ai, "attack least hp target") {}
        virtual string GetTargetName() { return "least hp target"; }
    };

    class AttackEnemyPlayerAction : public AttackAction
    {
    public:
        AttackEnemyPlayerAction(PlayerbotAI* ai) : AttackAction(ai, "attack enemy player") {}
        virtual string GetTargetName() { return "enemy player target"; }
    };

    class AttackRtiTargetAction : public AttackAction
    {
    public:
        AttackRtiTargetAction(PlayerbotAI* ai) : AttackAction(ai, "attack rti target") {}
        virtual string GetTargetName() { return "rti target"; }
    };

    class DropTargetAction : public Action
    {
    public:
        DropTargetAction(PlayerbotAI* ai) : Action(ai, "drop target") {}

        virtual bool Execute(Event event)
        {
            Unit* target = context->GetValue<Unit*>("current target")->Get();
            ObjectGuid pullTarget = context->GetValue<ObjectGuid>("pull target")->Get();
            list<ObjectGuid> possible = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets")->Get();
            if (pullTarget && find(possible.begin(), possible.end(), pullTarget) == possible.end())
            {
                context->GetValue<ObjectGuid>("pull target")->Set(ObjectGuid());
            }
            context->GetValue<Unit*>("current target")->Set(NULL);
            bot->SetSelectionGuid(ObjectGuid());
            ai->ChangeEngine(BOT_STATE_NON_COMBAT);
            ai->InterruptSpell();
            bot->AttackStop();
            Pet* pet = bot->GetPet();
            if (pet)
            {
#ifdef MANGOS
                CreatureAI*
#endif
#ifdef CMANGOS
                    UnitAI*
#endif
                    creatureAI = ((Creature*)pet)->AI();
                if (creatureAI)
                {
#ifdef CMANGOS
                    creatureAI->SetReactState(REACT_PASSIVE);
#endif
#ifdef MANGOS
                    pet->GetCharmInfo()->SetReactState(REACT_PASSIVE);
                    pet->GetCharmInfo()->SetCommandState(COMMAND_FOLLOW);
#endif
                    pet->AttackStop();
                }
            }
            if (!urand(0, 25))
            {
                vector<uint32> sounds;
                if (target && sServerFacade.UnitIsDead(target))
                {
                    sounds.push_back(TEXTEMOTE_CHEER);
                    sounds.push_back(TEXTEMOTE_CONGRATULATE);
                }
                else
                {
                    sounds.push_back(304); // guard
                    sounds.push_back(325); // stay
                }
                if (!sounds.empty()) ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
            }
            return true;
        }
    };

}
