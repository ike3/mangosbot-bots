#pragma once

#include "../Action.h"
#include "QuestAction.h"
#include "UseItemAction.h"

namespace ai
{
    class GreetAction : public Action
    {
    public:
        GreetAction(PlayerbotAI* ai);
        virtual bool Execute(Event event);
    };
}

namespace ai
{
   class ImbueWithPoisonAction : public Action
   {
   public:
      ImbueWithPoisonAction(PlayerbotAI* ai);
      virtual bool Execute(Event event);
   };
}

namespace ai
{
   class ImbueWithStoneAction : public Action
   {
   public:
      ImbueWithStoneAction(PlayerbotAI* ai);
      virtual bool Execute(Event event);
   };
}

namespace ai
{
   class ImbueWithOilAction : public Action
   {
   public:
      ImbueWithOilAction(PlayerbotAI* ai);
      virtual bool Execute(Event event);
   };
}
