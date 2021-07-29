#pragma once

#include "../Action.h"
#include "../../ServerFacade.h"

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
      bool UseItem(Item* item, ObjectGuid go, Item* itemTarget);
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
   };

   class UseManaPotion : public UseItemAction
   {
   public:
      UseManaPotion(PlayerbotAI* ai) : UseItemAction(ai, "mana potion") {}
      virtual bool isUseful() { return AI_VALUE2(bool, "combat", "self target"); }
   };

   class UseHearthStone : public UseItemAction
   {
   public:
       UseHearthStone(PlayerbotAI* ai) : UseItemAction(ai, "hearthstone", true) {}

       bool isUseful() { return !bot->IsInCombat() && !bot->InBattleGround(); }
       
       virtual bool Execute(Event event);
   };

   class UseRandomRecipe : public UseItemAction
   {
   public:
       UseRandomRecipe(PlayerbotAI* ai) : UseItemAction(ai, "random recipe", true) {}

       virtual bool isUseful();

       virtual bool isPossible() { return true; }
       
       virtual bool Execute(Event event);
   };
}
