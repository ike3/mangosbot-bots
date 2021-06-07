#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class MoveTarget
    {
    public:
        MoveTarget(string targetName = "", float relevance = 0) : targetName(targetName), relevance(relevance) {}
        virtual string getName() { return targetName; }
        virtual float getBaseRelevance() { return relevance; }
        virtual float getRelevance() { return isInRange() ? relevance : 1; }
        virtual WorldPosition getPos() { return WorldPosition(); }

        virtual float getDist(WorldPosition pos) { return pos.distance(getPos()); }
        virtual bool isInRange(WorldPosition pos) { return getDist(pos) < maxDist; }
        virtual bool isInRange() { return false; }
        virtual bool doAction() { return true; }
    private:
        string targetName = "";
        float relevance = 0;
        float minDist = sPlayerbotAIConfig.targetPosRecalcDistance;
        float maxDist = sPlayerbotAIConfig.followDistance;
    };

    template<class T>
    class ObjectMoveTarget : public MoveTarget , public AiObject
    {
    public:    
        ObjectMoveTarget(PlayerbotAI* ai, T value, string targetName = "", float relevance = 0) : AiObject(ai), MoveTarget(targetName, relevance), value(value) {};
        virtual WorldPosition getPos() { return WorldPosition(value); }
        virtual bool isInRange() { return MoveTarget::isInRange(WorldPosition(bot)); }
        virtual T Get() { return value; }
    protected:
        T value;
    };

    template<class T>
    class ObjectMoveActionTarget : public ObjectMoveTarget<T> , public Qualified
    {
    public:
        ObjectMoveActionTarget(PlayerbotAI* ai, T value, string targetName = "", float relevance = 0, string actionName = "", Event actionEvent = Event(), string qualifier = "") : ObjectMoveTarget(ai, value, targetName, relevance), Qualified(qualifier), actionName(actionName), actionEvent(actionEvent)  {};
        virtual bool doAction() { return ai->DoSpecificAction(actionName, actionEvent, true, getQualifier()); }
    protected:
        string actionName;
        Event actionEvent;
    };
    
    class MoveTargetValue : public ManualSetValue<MoveTarget*>
    {
    public:
        MoveTargetValue(PlayerbotAI* ai, MoveTarget* defaultValue = new MoveTarget()) : ManualSetValue<MoveTarget*>(ai, defaultValue) { }

        virtual ~MoveTargetValue() { delete value; }
    };
}
