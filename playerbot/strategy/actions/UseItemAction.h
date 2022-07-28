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
          if (/*isRandomBot && */sPlayerbotAIConfig.freeFood)
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

              if (spellId && bot->IsSpellReady(spellId, proto))
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
          if (/*isRandomBot && */sPlayerbotAIConfig.freeFood)
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

              if (spellId && bot->IsSpellReady(spellId, proto))
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

   // goblin sappers
   class CastGoblinSappersAction : public UseItemAction
   {
   public:
       CastGoblinSappersAction(PlayerbotAI* ai) : UseItemAction(ai, "goblin sapper") {}
       virtual bool isPossible() { return true; }
       virtual bool isUseful()
       {
           return bot->GetLevel() >= 50 && bot->GetHealth() > 1000;
       }
       virtual bool Execute(Event event)
       {
           uint32 goblinSapper = 10646;
           bool added = bot->HasItemCount(goblinSapper, 1);
           if (!added)
               added = bot->StoreNewItemInBestSlots(goblinSapper, 10);

           if (!added)
               return false;

           list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "goblin sapper charge");
           list<Item*>::iterator i = items.begin();
           Item* item = *i;

           if (!item)
               return false;

           return UseItemAuto(item);
       }
   };

   // oil of immolation
   class CastOilOfImmolationAction : public UseItemAction
   {
   public:
       CastOilOfImmolationAction(PlayerbotAI* ai) : UseItemAction(ai, "oil of immolation") {}
       virtual bool isPossible() { return true; }
       virtual bool isUseful()
       {
           return bot->GetLevel() >= 31 && !ai->HasAura(11350, bot);
       }
       virtual bool Execute(Event event)
       {
           uint32 oil = 8956;
           bool added = bot->HasItemCount(oil, 1);
           if (!added)
               added = bot->StoreNewItemInBestSlots(oil, 1);

           if (!added)
               return false;

           list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "oil of immolation");
           list<Item*>::iterator i = items.begin();
           Item* item = *i;

           if (!item)
               return false;

           return UseItemAuto(item);
       }
   };

   // dark rune
   class DarkRuneAction : public UseItemAction
   {
   public:
       DarkRuneAction(PlayerbotAI* ai) : UseItemAction(ai, "dark rune") {}
       virtual bool isPossible() { return true; }
       virtual bool isUseful()
       {
           if (bot->getClass() == CLASS_MAGE) // mage should use mana gem, shares cd with dark rune
               return false;

           return bot->GetLevel() == 60 && AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.lowHealth;
       }
       virtual bool Execute(Event event)
       {
           uint32 rune = 20520;
           bool added = bot->HasItemCount(rune, 1);
           if (!added)
               added = bot->StoreNewItemInBestSlots(rune, 20);

           if (!added)
               return false;

           list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "dark rune");
           list<Item*>::iterator i = items.begin();
           Item* item = *i;

           if (!item)
               return false;

           return UseItemAuto(item);
       }
   };
}
