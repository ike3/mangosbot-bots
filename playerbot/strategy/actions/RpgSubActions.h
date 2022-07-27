#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "ChooseRpgTargetAction.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class RpgHelper : public AiObject
    {
    public:
        RpgHelper(PlayerbotAI* ai) : AiObject(ai) {}

        void OnExecute(string nextAction = "rpg") { if (ai->HasRealPlayerMaster() && nextAction == "rpg") nextAction = "rpg cancel"; SET_AI_VALUE(string, "next rpg action", nextAction); };
        void BeforeExecute();
        void AfterExecute(bool doDelay = true,  bool waitForGroup = false, string nextAction = "rpg");

        virtual GuidPosition guidP() { return AI_VALUE(GuidPosition, "rpg target"); }
        virtual ObjectGuid guid() { return (ObjectGuid)guidP(); }        
        virtual bool InRange() { return guidP() ? (guidP().sqDistance2d(bot) < INTERACTION_DISTANCE * INTERACTION_DISTANCE) : false; }
    private:
        void setFacingTo(GuidPosition guidPosition);
        void setFacing(GuidPosition guidPosition);
        void setDelay(bool waitForGroup);
    };

    class RpgEnabled
    {
    public:
        RpgEnabled(PlayerbotAI* ai) { rpg = make_unique<RpgHelper>(ai); }
    protected:
        unique_ptr<RpgHelper> rpg;
    };

    class RpgSubAction : public Action, public RpgEnabled
    {
    public:
        RpgSubAction(PlayerbotAI* ai, string name = "rpg sub") : Action(ai, name), RpgEnabled(ai) {}

        //Long range is possible?
        virtual bool isPossible() { return rpg->guidP() && rpg->guidP().GetWorldObject();}
        //Short range can we do the action now?
        virtual bool isUseful() { return rpg->InRange(); }

        virtual bool Execute(Event event) { bool doAction = ai->DoSpecificAction(ActionName(), ActionEvent(event), true); rpg->AfterExecute(doAction, true); return doAction; }
    protected:
        virtual string ActionName() { return "none"; }
        virtual Event ActionEvent(Event event) { return event; }
    };        

    class RpgStayAction : public RpgSubAction
    {
    public:
        RpgStayAction(PlayerbotAI* ai, string name = "rpg stay") : RpgSubAction(ai, name) {}

        virtual bool isUseful() { return rpg->InRange() && !ai->HasRealPlayerMaster(); }

        virtual bool Execute(Event event) { if (bot->GetPlayerMenu()) bot->GetPlayerMenu()->CloseGossip(); rpg->AfterExecute();  return true; };
    };   

    class RpgWorkAction : public RpgSubAction
    {
    public:
        RpgWorkAction(PlayerbotAI* ai, string name = "rpg work") : RpgSubAction(ai, name ) {}

        virtual bool isUseful() { return rpg->InRange() && !ai->HasRealPlayerMaster(); }

        virtual bool Execute(Event event) { bot->HandleEmoteCommand(EMOTE_STATE_USESTANDING); rpg->AfterExecute(); return true; };
    };

    class RpgEmoteAction : public RpgSubAction
    {
    public:
        RpgEmoteAction(PlayerbotAI* ai, string name = "rpg emote") : RpgSubAction(ai, name) {}

        virtual bool isUseful() { Unit* unit = rpg->guidP().GetUnit(); if (unit && unit->GetName() == "chicken") return rpg->InRange(); return rpg->InRange() && !ai->HasRealPlayerMaster(); }

        virtual bool Execute(Event event);
    };

    class RpgCancelAction : public RpgSubAction
    {
    public:
        RpgCancelAction(PlayerbotAI* ai, string name = "rpg cancel") : RpgSubAction(ai, name) {}

        virtual bool Execute(Event event) { RESET_AI_VALUE(GuidPosition,"rpg target"); rpg->OnExecute(""); return true; };
    };

    class RpgTaxiAction : public RpgSubAction
    {
    public:
        RpgTaxiAction(PlayerbotAI* ai, string name = "rpg taxi") : RpgSubAction(ai, name) {}

        virtual bool isUseful() { return rpg->InRange() && !ai->HasRealPlayerMaster(); }

        virtual bool Execute(Event event);
    };

    class RpgDiscoverAction : public RpgTaxiAction
    {
    public:
        RpgDiscoverAction(PlayerbotAI* ai, string name = "rpg discover") : RpgTaxiAction(ai,name) {}

        virtual bool Execute(Event event);
    };

    class RpgStartQuestAction : public RpgSubAction
    {
    public:
        RpgStartQuestAction(PlayerbotAI* ai, string name = "rpg start quest") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "accept all quests"; }
        virtual Event ActionEvent(Event event) {WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST); p << rpg->guid(); p.rpos(0);  return  Event("rpg action", p); }
    };

    class RpgEndQuestAction : public RpgSubAction
    {
    public:
        RpgEndQuestAction(PlayerbotAI* ai, string name = "rpg end quest") : RpgSubAction(ai, name) {}
        virtual bool Execute(Event event) { bool doAction = ai->DoSpecificAction(ActionName(), ActionEvent(event), true); rpg->AfterExecute(doAction, true, "rpg start quest"); return doAction; }

    private:
        virtual string ActionName() { return "talk to quest giver"; }
        virtual Event ActionEvent(Event event) { WorldPacket p(CMSG_QUESTGIVER_COMPLETE_QUEST); p << rpg->guid(); p.rpos(0);  return  Event("rpg action", p); }
    };

    class RpgBuyAction : public RpgSubAction
    {
    public:
        RpgBuyAction(PlayerbotAI* ai, string name = "rpg buy") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "buy"; }
        virtual Event ActionEvent(Event event) { return Event("rpg action", "vendor"); }
    };
   
    class RpgSellAction : public RpgSubAction
    {
    public:
        RpgSellAction(PlayerbotAI* ai, string name = "rpg sell") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "sell"; }
        virtual Event ActionEvent(Event event) { return Event("rpg action", "vendor"); }
    };

    class RpgRepairAction : public RpgSubAction
    {
    public:
        RpgRepairAction(PlayerbotAI* ai, string name = "rpg repair") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "repair"; }
    };

    class RpgTrainAction : public RpgSubAction
    {
    public:
        RpgTrainAction(PlayerbotAI* ai, string name = "rpg train") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "trainer"; }
    };

    class RpgHealAction : public RpgSubAction
    {
    public:
        RpgHealAction(PlayerbotAI* ai, string name = "rpg heal") : RpgSubAction(ai, name) {}

        virtual bool Execute(Event event);
    };

    class RpgHomeBindAction : public RpgSubAction
    {
    public:
        RpgHomeBindAction(PlayerbotAI* ai, string name = "rpg home bind") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "home"; }
    };

    class RpgQueueBgAction : public RpgSubAction
    {
    public:
        RpgQueueBgAction(PlayerbotAI* ai, string name = "rpg queue bg") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { SET_AI_VALUE(uint32, "bg type", (uint32)AI_VALUE(BattleGroundTypeId, "rpg bg type")); return "free bg join"; }
    };

    class RpgBuyPetitionAction : public RpgSubAction
    {
    public:
        RpgBuyPetitionAction(PlayerbotAI* ai, string name = "rpg buy petition") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "buy petition"; }
    };

    class RpgUseAction : public RpgSubAction
    {
    public:
        RpgUseAction(PlayerbotAI* ai, string name = "rpg use") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "use"; }
        virtual Event ActionEvent(Event event) { return Event("rpg action", chat->formatWorldobject(rpg->guidP().GetWorldObject())); }
    };

    class RpgSpellAction : public RpgSubAction
    {
    public:
        RpgSpellAction(PlayerbotAI* ai, string name = "rpg spell") : RpgSubAction(ai, name) {}

    private:
        virtual bool isUseful() { return false; }
        virtual string ActionName() { return "cast random spell"; }
        virtual Event ActionEvent(Event event) { return Event("rpg action", chat->formatWorldobject(rpg->guidP().GetWorldObject())); }
    };

    class RpgCraftAction : public RpgSubAction
    {
    public:
        RpgCraftAction(PlayerbotAI* ai, string name = "rpg craft") : RpgSubAction(ai, name) {}

    private:
        virtual string ActionName() { return "craft random item"; }
        virtual Event ActionEvent(Event event) { return Event("rpg action", chat->formatWorldobject(rpg->guidP().GetWorldObject())); }
    };

    class RpgTradeUsefulAction : public RpgSubAction
    {
    public:
        RpgTradeUsefulAction(PlayerbotAI* ai, string name = "rpg trade useful") : RpgSubAction(ai, name) {}

        list<Item*> CanGiveItems(GuidPosition guidPosition);

        virtual bool Execute(Event event);
    };

    class RpgDuelAction : public RpgSubAction
    {
    public:
        RpgDuelAction(PlayerbotAI* ai, string name = "rpg duel") : RpgSubAction(ai, name) {}

        virtual bool isUseful();
        virtual bool Execute(Event event);
    };

    class RpgMountAnimAction : public Action
    {
    public:
        RpgMountAnimAction(PlayerbotAI* ai, string name = "rpg mount anim") : Action(ai, name) {}

        virtual bool isUseful();
        virtual bool Execute(Event event);
    };
}
