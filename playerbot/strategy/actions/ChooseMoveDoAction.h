#pragma once

#include "MovementActions.h"
#include "../values/MoveTargetValue.h"

namespace ai
{
    template<class T>
    class ChooseMoveDoAction : public MovementAction, public Qualified {
    public:
        virtual void getCurrentTarget() { currentTarget = AI_VALUE(MoveTarget*, "move target"); }
        virtual void setCurrentTarget() {delete currentTarget; context->GetValue<MoveTarget*>("move target")->Set(newTarget.release()); getCurrentTarget();}
        virtual void clearCurrentTarget() { delete currentTarget; currentTarget = new MoveTarget(); context->GetValue<MoveTarget*>("move target")->Set(currentTarget); }

        ChooseMoveDoAction(PlayerbotAI* ai, string name = "choose move target", string targetValueName = "") : MovementAction(ai, name), targetValueName(targetValueName) {}

        virtual bool isUseful() { getCurrentTarget();  return isCurrentTarget() || isBetterTarget(); }

        virtual bool Execute(Event event) {
            if (!isCurrentTarget())
            {
                if (!isBetterTarget())
                    return doTargetAction();

                if (!SelectNewTarget(event))
                    return false;
            }
               
            if (!isAtCurrentTarget())
            {                
                return MoveToTarget();
            }                       

            bool isExecuted = ExecuteAction(event);

            clearCurrentTarget();

            return isExecuted;
        }

        virtual bool doTargetAction()
        {
            return currentTarget->doAction();
        }

        virtual bool SelectNewTarget(Event event)
        {
            if (!getPotentialTargets() || !FilterPotentialTargets() || !getPotentialTarget() || !getNewMoveTarget(event))
                return false;

            getCurrentTarget();

            bool isBetter = NewIsBetterTarget();

            if (isBetter)
                setCurrentTarget();

            return false; //isBetter continue checking for better values.
        }

        virtual bool MoveToTarget()
        {
            WorldPosition movePos = currentTarget->getPos();

            if (bot->IsWithinLOS(movePos.getX(), movePos.getY(), movePos.getZ()))
                return MoveNear(movePos.getMapId(), movePos.getX(), movePos.getY(), movePos.getZ(), 0);
            else
                return MoveTo(movePos.getMapId(), movePos.getX(), movePos.getY(), movePos.getZ(), false, false);
        }

        virtual bool ExecuteAction(Event event) { return true; }

        virtual string getTargetValueName() { return targetValueName; }

        virtual bool isCurrentTarget() { return currentTarget->getName() == getTargetValueName(); }

        virtual bool isBetterTarget() { return !isCurrentTarget() && currentTarget->getRelevance() <= getRelevance(); }

        virtual bool isAtCurrentTarget() { return currentTarget->isInRange(bot); }

        virtual bool getPotentialTargets() { if (getQualifier().empty()) potentialTargets = { AI_VALUE(T, getTargetValueName()) }; else potentialTargets = { AI_VALUE2(T, getTargetValueName(), getQualifier()) };  return !potentialTargets.empty(); }

        virtual bool FilterPotentialTargets() { return true; }

        virtual bool getPotentialTarget() { potentialTarget = potentialTargets.front();  return true; }

        virtual bool getNewMoveTarget(Event event) { newTarget = std::make_unique<ObjectMoveActionTarget<T>>(ai, potentialTarget, targetValueName, getRelevance(), getName(), event, getQualifier()); return true; }

        virtual bool NewIsBetterTarget()
        {
            WorldPosition botPos(bot);
            return newTarget->getBaseRelevance() > currentTarget->getBaseRelevance() || (newTarget->getBaseRelevance() == currentTarget->getBaseRelevance() && newTarget->getPos().distance(&botPos) < currentTarget->getPos().distance(&botPos));
        }

        ObjectMoveActionTarget<T>* getObjectTarget() { return dynamic_cast<ObjectMoveActionTarget<T>*>(currentTarget); }
    protected:
        string targetValueName = "";
        list<T> potentialTargets;
        T potentialTarget;
        MoveTarget* currentTarget;
        unique_ptr<ObjectMoveActionTarget<T>> newTarget;
    };

    class ContinueMoveDoAction : public ChooseMoveDoAction<Unit*> {
    public:
        ContinueMoveDoAction(PlayerbotAI* ai, string name = "continue action", string targetValueName = "") : ChooseMoveDoAction(ai, name, targetValueName) {}
        virtual bool isUseful() { getCurrentTarget(); return !isCurrentTarget();} //Only continue non-current actions.
    };

    //Simple example of a single target selection.
    //Will set the current move target to "self target" assuming no other move target has priority > 1
    class MoveSelfAction : public ChooseMoveDoAction<Unit*> {
        MoveSelfAction(PlayerbotAI* ai, string name = "choose self target", string targetValueName = "self target") : ChooseMoveDoAction(ai, name, targetValueName) {}
    };

    //Template for ai-values that returns a list of targets. 
    //The closest target to the bot is selected to compare to current move target.
    template<class T>
    class ChooseMoveDoListAction : public ChooseMoveDoAction<T> {
    public:
        ChooseMoveDoListAction(PlayerbotAI* ai, string name = "choose list action", string targetValueName = "") : ChooseMoveDoAction(ai, name, targetValueName) {}

        //Default select all targets from the ai-value.
        virtual bool getPotentialTargets() { if (getQualifier().empty()) potentialTargets = AI_VALUE(list<T>, getTargetValueName()); else potentialTargets = AI_VALUE2(list<T>, getTargetValueName(), getQualifier());  return !potentialTargets.empty(); }

        //Default pick the closest potential target from the list.
        virtual bool getPotentialTarget() {return !potentialTargets.empty() && (potentialTarget = WorldPosition(bot).closest<T>(potentialTargets).first); }
    };

    //Simple example of a multiple target selection.
    class AttackPossibleTargetsAction : public ChooseMoveDoListAction<ObjectGuid> {
    public:
        AttackPossibleTargetsAction(PlayerbotAI* ai, string name = "choose possible targets", string targetValueName = "possible targets") : ChooseMoveDoListAction(ai, name, targetValueName) {}

        virtual bool ExecuteAction(Event event) {/*attack code here*/ return true; }
    };
}
