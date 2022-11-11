#pragma once

#include "Engine.h"

namespace ai
{
    struct Reaction
    {
        bool IsValid() const { return action != nullptr; }
        bool IsActive() const { return IsValid() ? duration > 100U : false; }
        void Reset() { action = nullptr; duration = 0U; }

        bool Update(uint32 elapsed);
        
        void SetAction(Action* inAction);
        Action* GetAction() const { return action; }
        
        void SetEvent(const Event& inEvent) { event = Event(inEvent); }
        Event& GetEvent() { return event; }
        
        uint32 GetDuration() const { return duration; }
        void SetDuration(uint32 inDuration) { duration = inDuration;}

    private:
        Event event;
        Action* action = nullptr;
        uint32 duration = 0U;
    };

    class ReactionEngine : public Engine
    {
    public:
        ReactionEngine(PlayerbotAI* ai, AiObjectContext* factory);

        void Reset();
        bool Update(uint32 elapsed, bool minimal, bool& reactionFound, bool& reactionFinished);
        bool IsReacting() const { return ongoingReaction.IsValid(); }
        bool HasIncomingReaction() const { return incomingReaction.IsValid(); }
        uint32 GetReactionDuration() const { return ongoingReaction.GetDuration(); }
        void SetReactionDuration(const Action* action, uint32 duration);

    private:
        bool FindReaction();
        bool StartReaction();
        void StopReaction();

        bool CanUpdateAIReaction() const { return aiReactionUpdateDelay < 100U; }

        Action* InitializeAction(ActionNode* actionNode) override;
        bool ListenAndExecute(Action* action, Event& event) override;

    protected:
        Reaction incomingReaction;
        Reaction ongoingReaction;

    private:
        uint32 aiReactionUpdateDelay;
    };
}
