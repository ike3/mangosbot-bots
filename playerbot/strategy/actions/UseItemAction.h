#pragma once
#include "GenericActions.h"
#include "../../ServerFacade.h"
#include "../../RandomItemMgr.h"

namespace ai
{
    class UseItemAction : public ChatCommandAction 
    {
    public:
        UseItemAction(PlayerbotAI* ai, string name = "use", bool selfOnly = false) : ChatCommandAction(ai, name), selfOnly(selfOnly) {}

    public:
        virtual bool isPossible() override;

        // Used when this action is executed as a reaction
        virtual bool ShouldReactionInterruptCast() const override { return true; }

    protected:
        virtual bool ExecuteCommand(Event& event) override;
        bool UseItemAuto(Item* item);
        bool UseItemOnGameObject(Item* item, ObjectGuid go);
        bool UseItemOnItem(Item* item, Item* itemTarget);
        bool UseItem(Item* item, ObjectGuid go, Item* itemTarget, Unit* unitTarget = nullptr);
        bool UseGameObject(ObjectGuid guid);
        bool SocketItem(Item * item, Item * gem, bool replace = false);
        void TellConsumableUse(Item* item, string action, float percent);

    private:
        bool selfOnly;
    };

    class UseItemIdAction : public UseItemAction
    {
    public:
        UseItemIdAction(PlayerbotAI* ai, string name, bool selfOnly = false) : UseItemAction(ai, name, selfOnly) {}
        virtual bool isPossible() override;

    protected:
        virtual bool ExecuteCommand(Event& event) override;
        virtual uint32 GetItemId() { return  0; }
        virtual Unit* GetTarget() override { return nullptr; }
        bool HasSpellCooldown(const uint32 itemId);
        bool CastItemSpell(uint32 itemId, Unit* target);
    };

    class UseTargetedItemIdAction : public UseItemIdAction
    {
    public:
        UseTargetedItemIdAction(PlayerbotAI* ai, string name, bool selfOnly = false) : UseItemIdAction(ai, name, selfOnly) {}
        virtual Unit* GetTarget() override { return Action::GetTarget(); }
        virtual uint32 GetItemId() override { return  0; }
    };

    class UseSpellItemAction : public UseItemAction 
    {
    public:
        UseSpellItemAction(PlayerbotAI* ai, string name, bool selfOnly = false) : UseItemAction(ai, name, selfOnly) {}
        virtual bool isUseful() override;
    };

    class UsePotionAction : public UseItemIdAction
    {
    public:
        UsePotionAction(PlayerbotAI* ai, string name, SpellEffects effect) : UseItemIdAction(ai, name), effect(effect) {}

        bool isUseful() override { return AI_VALUE2(bool, "combat", "self target"); }

        virtual uint32 GetItemId() override
        {
            list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", getName());
            if (items.empty())
            {
                return sRandomItemMgr.GetRandomPotion(bot->GetLevel(), effect);
            }

            return items.front()->GetProto()->ItemId;
        }

    private:
        SpellEffects effect;
    };

    class UseHealingPotionAction : public UsePotionAction
    {
    public:
        UseHealingPotionAction(PlayerbotAI* ai) : UsePotionAction(ai, "healing potion", SPELL_EFFECT_HEAL) {}
    };

    class UseManaPotionAction : public UsePotionAction
    {
    public:
        UseManaPotionAction(PlayerbotAI* ai) : UsePotionAction(ai, "mana potion", SPELL_EFFECT_ENERGIZE) {}
    };

    class UseHearthStoneAction : public UseItemAction
    {
    public:
        UseHearthStoneAction(PlayerbotAI* ai) : UseItemAction(ai, "hearthstone", true) {}

        virtual bool ExecuteCommand(Event& event) override;

        bool isUseful() override { return !bot->InBattleGround() && sServerFacade.IsSpellReady(bot, 8690); }
    
        // Used when this action is executed as a reaction
        bool ShouldReactionInterruptMovement() const override { return true; }

        virtual uint32 getDuration() const override { return 10000U; };
    };

    class UseRandomRecipeAction : public UseItemAction
    {
    public:
        UseRandomRecipeAction(PlayerbotAI* ai) : UseItemAction(ai, "random recipe", true) {}

        virtual bool isUseful() override;
        virtual bool isPossible() override {return AI_VALUE2(uint32,"item count", "recipe") > 0; }
      
        virtual bool ExecuteCommand(Event& event) override;
        virtual uint32 getDuration() const override { return 3000U; };

        // Used when this action is executed as a reaction
        bool ShouldReactionInterruptMovement() const override { return true; }
    };

    class UseRandomQuestItemAction : public UseItemAction
    {
    public:
        UseRandomQuestItemAction(PlayerbotAI* ai) : UseItemAction(ai, "random quest item", true) {}

        virtual bool isUseful() override;
        virtual bool isPossible() override { return AI_VALUE2(uint32, "item count", "quest") > 0;}

        virtual bool ExecuteCommand(Event& event) override;

        // Used when this action is executed as a reaction
        bool ShouldReactionInterruptMovement() const override { return true; }
    };

    // goblin sappers
    class UseGoblinSapperChargeAction : public UseItemIdAction
    {
    public:
        UseGoblinSapperChargeAction(PlayerbotAI* ai) : UseItemIdAction(ai, "goblin sapper") {}
        virtual bool isUseful() override { return bot->GetSkillValue(202) >= 205 && bot->GetHealth() > 1000; }

        virtual uint32 GetItemId() override
        { 
#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                return false;
#endif
            return (bot->GetLevel() >= 68) ? 23827 : 10646; 
        }
    };

    // oil of immolation
    class UseOilOfImmolationAction : public UseItemIdAction
    {
    public:
        UseOilOfImmolationAction(PlayerbotAI* ai) : UseItemIdAction(ai, "oil of immolation") {}
        virtual bool isUseful() override
        {
#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                return false;
#endif
            return bot->GetLevel() >= 31 && !ai->HasAura(11350, bot);
        }

        virtual uint32 GetItemId() override { return 8956; }
    };

    class UseBgBannerAction : public UseItemIdAction
    {
    public:
        UseBgBannerAction(PlayerbotAI* ai) : UseItemIdAction(ai, "bg banner") {}

        virtual bool isUseful() override
        {
            if (!bot->InBattleGround() || bot->GetLevel() < 60 || !bot->IsInCombat())
                return false;

            list<ObjectGuid> units = *context->GetValue<list<ObjectGuid> >("nearest npcs no los");
            for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); i++)
            {
                Unit* unit = ai->GetUnit(*i);
                if (!unit)
                    continue;

                if (bot->GetTeam() == HORDE && unit->GetEntry() == 14466)
                    return false;

                if (bot->GetTeam() == ALLIANCE && unit->GetEntry() == 14465)
                    return false;
            }

            return true;
        }

        virtual uint32 GetItemId() override
        {
            return bot->GetTeam() == ALLIANCE ? 18606 : 18607;
        }

        // Used when this action is executed as a reaction
        bool ShouldReactionInterruptMovement() const override { return true; }
    };

    class UseBandageAction : public UseTargetedItemIdAction
    {
    public:
        UseBandageAction(PlayerbotAI* ai) : UseTargetedItemIdAction(ai, "use bandage") {}

        virtual bool isUseful() override
        {
            if (bot->HasAura(11196))
                return false;

            if (AI_VALUE(uint8, "my attacker count") > 0 || bot->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE))
                return false;

            if (bot->GetSkillValue(129) < 1)
                return false;

            return true;
        }

        virtual string GetTargetName() override { return "self target"; }

        virtual uint32 GetItemId() override
        {
            if (bot->GetSkillValue(129) >= 225)
                return 14530;
            if (bot->GetSkillValue(129) >= 200)
                return 14529;
            if (bot->GetSkillValue(129) >= 175)
                return 8545;
            if (bot->GetSkillValue(129) >= 150)
                return 8544;
            if (bot->GetSkillValue(129) >= 125)
                return 6451;
            if (bot->GetSkillValue(129) >= 100)
                return 6450;
            if (bot->GetSkillValue(129) >= 75)
                return 3531;
            if (bot->GetSkillValue(129) >= 50)
                return 3530;
            if (bot->GetSkillValue(129) >= 20)
                return 2581;
            return 1251;
        }

        // Used when this action is executed as a reaction
        bool ShouldReactionInterruptMovement() const override { return true; }

        virtual uint32 getDuration() const override { return 8000U; };
    };

    class UseAdamantiteGrenadeAction : public UseTargetedItemIdAction
    {
    public:
        UseAdamantiteGrenadeAction(PlayerbotAI* ai) : UseTargetedItemIdAction(ai, "adamantite grenade") {}

        virtual string GetTargetName() override { return "current target"; }

        virtual bool isUseful() override
        {
#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                return false;
#endif

            Unit* target = GetTarget();
            if (!target)
                return false;

            if (!target->IsNonMeleeSpellCasted(false) && target->IsStopped())
                return false;

            if (bot->GetSkillValue(202) < 175)
                return false;

            return bot->GetLevel() >= 52;
        }

        virtual uint32 GetItemId() 
        { 
            if (bot->GetSkillValue(202) >= 325)
                return 23737;
            if (bot->GetSkillValue(202) >= 260)
                return 15993;
            return 4390; 
        }
    };

    // dark rune
    class UseDarkRuneAction : public UseItemIdAction
    {
    public:
        UseDarkRuneAction(PlayerbotAI* ai) : UseItemIdAction(ai, "dark rune") {}

        virtual bool isUseful() override
        {
#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                return false;
#endif

            if (bot->getClass() == CLASS_MAGE) // mage should use mana gem, shares cd with dark rune
                return false;

            return AI_VALUE2(uint8, "health", "self target") > 1000;
        }

        virtual uint32 GetItemId() override { return 20520; }
    };
}
