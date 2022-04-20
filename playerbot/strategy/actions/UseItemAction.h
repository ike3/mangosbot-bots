#pragma once

#include "../Action.h"
#include "../../ServerFacade.h"
#include "../../RandomItemMgr.h"

namespace ai
{
   class UseItemAction : public Action {
   public:
      UseItemAction(PlayerbotAI* ai, string name = "use", bool selfOnly = false) : Action(ai, name), selfOnly(selfOnly) {}

   public:
      virtual bool Execute(Event event);
      virtual bool isPossible();

   protected:
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

   class UseSpellItemAction : public UseItemAction {
   public:
      UseSpellItemAction(PlayerbotAI* ai, string name, bool selfOnly = false) : UseItemAction(ai, name, selfOnly) {}

   public:
      virtual bool isUseful();
   };

   class UseHealingPotion : public UseItemAction {
   public:
      UseHealingPotion(PlayerbotAI* ai) : UseItemAction(ai, "healing potion") {}
      virtual bool isUseful() { return AI_VALUE2(bool, "combat", "self target"); }
      virtual bool Execute(Event event)
      {
          bool isRandomBot = sRandomPlayerbotMgr.IsRandomBot(bot);
          if (isRandomBot && sPlayerbotAIConfig.freeFood)
          {
              if (bot->IsNonMeleeSpellCasted(true))
                  return false;

              uint32 itemId = sRandomItemMgr.GetRandomPotion(bot->GetLevel(), SPELL_EFFECT_HEAL);
              if (!itemId)
              {
                  return false;
              }

              ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);
              if (!proto)
                  return false;

              uint32 spellId = 0;
              for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
              {
                  if (proto->Spells[i].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                      continue;

                  if (proto->Spells[i].SpellId > 0)
                  {
                      spellId = proto->Spells[i].SpellId;
                  }
              }

              if (spellId && bot->IsSpellReady(spellId))
              {
                  ai->CastSpell(spellId, bot);
                  return true;
              }
          }

          return UseItemAction::Execute(event);
      }
   };

   class UseManaPotion : public UseItemAction
   {
   public:
      UseManaPotion(PlayerbotAI* ai) : UseItemAction(ai, "mana potion") {}
      virtual bool isUseful() { return AI_VALUE2(bool, "combat", "self target"); }
      virtual bool Execute(Event event)
      {
          bool isRandomBot = sRandomPlayerbotMgr.IsRandomBot(bot);
          if (isRandomBot && sPlayerbotAIConfig.freeFood)
          {
              if (bot->IsNonMeleeSpellCasted(true))
                  return false;

              uint32 itemId = sRandomItemMgr.GetRandomPotion(bot->GetLevel(), SPELL_EFFECT_ENERGIZE);
              if (!itemId)
              {
                  return false;
              }

              ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);
              if (!proto)
                  return false;

              uint32 spellId = 0;
              for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
              {
                  if (proto->Spells[i].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                      continue;

                  if (proto->Spells[i].SpellId > 0)
                  {
                      spellId = proto->Spells[i].SpellId;
                  }
              }

              if (spellId && bot->IsSpellReady(spellId))
              {
                  ai->CastSpell(spellId, bot);
                  return true;
              }
          }

          return UseItemAction::Execute(event);
      }
   };

   class UseHearthStone : public UseItemAction
   {
   public:
       UseHearthStone(PlayerbotAI* ai) : UseItemAction(ai, "hearthstone", true) {}

       bool isUseful() { return !bot->InBattleGround(); }
       
       virtual bool Execute(Event event);
   };

   class UseRandomRecipe : public UseItemAction
   {
   public:
       UseRandomRecipe(PlayerbotAI* ai) : UseItemAction(ai, "random recipe", true) {}

       virtual bool isUseful();
       virtual bool isPossible() {return AI_VALUE2(uint32,"item count", "recipe") > 0; }
      
       virtual bool Execute(Event event);
   };

   class UseRandomQuestItem : public UseItemAction
   {
   public:
       UseRandomQuestItem(PlayerbotAI* ai) : UseItemAction(ai, "random quest item", true) {}

       virtual bool isUseful();
       virtual bool isPossible() { return AI_VALUE2(uint32, "item count", "quest") > 0;}

       virtual bool Execute(Event event);
   };
}
