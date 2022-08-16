#pragma once

#include "../Action.h"
#include "GenericSpellActions.h"
#include "ReachTargetActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"

namespace ai
{
    class MeleeAction : public AttackAction 
    {
    public:
        MeleeAction(PlayerbotAI* ai) : AttackAction(ai, "melee") {}

        virtual string GetTargetName() { return "current target"; }

        virtual bool isUseful();
    };

    class UseLightwellAction : public MovementAction
    {
    public:
        UseLightwellAction(PlayerbotAI* ai) : MovementAction(ai, "lightwell") {}
        virtual bool isUseful()
        {
            return (bot->getClass() == CLASS_PRIEST || bot->GetGroup()) && bot->GetHealthPercent() < sPlayerbotAIConfig.mediumHealth && !ai->HasAura("lightwell renew", bot);
        }
        virtual ActionThreatType getThreatType() { return ACTION_THREAT_NONE; }
        virtual bool Execute(Event event)
        {
            list<ObjectGuid> closeObjects = AI_VALUE(list<ObjectGuid>, "nearest game objects no los");
            if (closeObjects.empty())
                return false;

            for (list<ObjectGuid>::iterator i = closeObjects.begin(); i != closeObjects.end(); ++i)
            {
                GameObject* go = ai->GetGameObject(*i);
                if (!go)
                    continue;

                if (!(go->GetEntry() == 181106 || go->GetEntry() == 181165 || go->GetEntry() == 181102 || go->GetEntry() == 181105))
                    continue;

                if (!sServerFacade.isSpawned(go) || !go->GetGoState() == GO_STATE_READY || !bot->CanInteract(go))
                    continue;

                // handle summoned traps, usually by players
                if (Unit* owner = go->GetOwner())
                {
                    if (owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* ownerPlayer = (Player*)owner;
                        if (!ownerPlayer)
                            return false;

                        if (!ownerPlayer->IsInGroup(bot))
                            continue;
                    }
                }

                if (bot->IsWithinDistInMap(go, INTERACTION_DISTANCE, false))
                {
                    WorldPacket data(CMSG_GAMEOBJ_USE);
                    data << go->GetObjectGuid();
                    bot->GetSession()->HandleGameObjectUseOpcode(data);
                    return true;
                }
                else
                {
                    return MoveNear(go, 4.0f);
                }
            }
            return false;
        }
    };
}
