#pragma once
#include "MovementActions.h"
#include "GenericActions.h"

namespace ai
{
    class ReturnPositionResetAction : public ChatCommandAction
    {
    public:
        ReturnPositionResetAction(PlayerbotAI* ai, string name) : ChatCommandAction(ai, name) {}
        void ResetReturnPosition();
        void SetReturnPosition(float x, float y, float z);
    };

    class FollowChatShortcutAction : public MovementAction
    {
    public:
        FollowChatShortcutAction(PlayerbotAI* ai) : MovementAction(ai, "follow chat shortcut") {}
        virtual bool Execute(Event& event);
        virtual bool isPossible() { return true; }
    };

    class StayChatShortcutAction : public ReturnPositionResetAction
    {
    public:
        StayChatShortcutAction(PlayerbotAI* ai) : ReturnPositionResetAction(ai, "stay chat shortcut") {}
        virtual bool Execute(Event& event) override;
    };

    class FleeChatShortcutAction : public ReturnPositionResetAction
    {
    public:
        FleeChatShortcutAction(PlayerbotAI* ai) : ReturnPositionResetAction(ai, "flee chat shortcut") {}
        virtual bool Execute(Event& event) override;
    };

    class GoawayChatShortcutAction : public ReturnPositionResetAction
    {
    public:
        GoawayChatShortcutAction(PlayerbotAI* ai) : ReturnPositionResetAction(ai, "runaway chat shortcut") {}
        virtual bool Execute(Event& event) override;
    };

    class GrindChatShortcutAction : public ReturnPositionResetAction
    {
    public:
        GrindChatShortcutAction(PlayerbotAI* ai) : ReturnPositionResetAction(ai, "grind chat shortcut") {}
        virtual bool Execute(Event& event) override;
    };

    class TankAttackChatShortcutAction : public ReturnPositionResetAction
    {
    public:
        TankAttackChatShortcutAction(PlayerbotAI* ai) : ReturnPositionResetAction(ai, "tank attack chat shortcut") {}
        virtual bool Execute(Event& event) override;
    };

    class MaxDpsChatShortcutAction : public ChatCommandAction
    {
    public:
        MaxDpsChatShortcutAction(PlayerbotAI* ai) : ChatCommandAction(ai, "max dps chat shortcut") {}
        virtual bool Execute(Event& event) override;
    };
}
