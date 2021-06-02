#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class MoveTarget
    {
    public:
        MoveTarget(string targetName = "", uint32 relevance = 0) : targetName(targetName), relevance(relevance) {}
        virtual string getName() { return targetName; }
        virtual uint32 getRelevance() { return relevance; }
        virtual WorldPosition getPos() { return WorldPosition(); }

        virtual float getDist(WorldPosition pos) { return pos.distance(getPos()); }
        virtual bool isInRange(WorldPosition pos) { return getDist(pos) < maxDist; }
        virtual bool isInRange(Player* bot) { return isInRange(WorldPosition(bot)); }
        virtual bool doAction(PlayerbotAI* ai) { return true; }
    private:
        string targetName = "";
        uint32 relevance = 0;
        float minDist = sPlayerbotAIConfig.targetPosRecalcDistance;
        float maxDist = sPlayerbotAIConfig.followDistance;
    };

    template<class T>
    class ObjectMoveTarget : public MoveTarget
    {
    public:    
        ObjectMoveTarget(T value, string targetName = "", uint32 priority = 0) : MoveTarget(targetName,priority), value(value) {};
        virtual WorldPosition getPos() { return WorldPosition(value); }
        virtual T Get() { return value; }
    protected:
        T value;
    };

    template<class T>
    class ObjectMoveActionTarget : public ObjectMoveTarget<T>
    {
    public:
        ObjectMoveActionTarget(T value, string targetName = "", uint32 priority = 0, string actionName = "", Event actionEvent = Event()) : ObjectMoveTarget(value, targetName, priority), actionName(actionName), actionEvent(actionEvent)  {};
        virtual bool doAction(PlayerbotAI* ai) {return ai->DoSpecificAction(actionName, actionEvent, true); }
    protected:
        string actionName;
        Event actionEvent;
    };
    
    class MoveTargetValue : public ManualSetValue<MoveTarget*>
    {
    public:
        MoveTargetValue(PlayerbotAI* ai, MoveTarget* defaultValue = new MoveTarget()) : ManualSetValue<MoveTarget*>(ai, defaultValue) {}

        virtual ~MoveTargetValue() { delete value; }
    };
}
