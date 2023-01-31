#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "UseItemAction.h"

namespace ai
{
    class CheckMountStateAction : public UseItemAction 
    {
    public:
        CheckMountStateAction(PlayerbotAI* ai) : UseItemAction(ai, "check mount state", true) {}

        virtual bool Execute(Event& event) override;
        virtual bool isPossible() { return true; }
        virtual bool isUseful();
    private:
        virtual bool CanFly() const;
        bool CanMountInBg() const;
        float GetAttackDistance() const;

        static uint32 MountSpeed(const SpellEntry* const spellInfo, const bool canFly = false);
        static uint32 MountSpeed(const ItemPrototype* proto, const bool canFly = false);

        vector<uint32> GetBestMountSpells(const bool canFly) const;
        vector<Item*> GetBestMounts(const bool canFly) const;
    public:
        uint32 GetBestMountSpeed(const bool canFly = false) const;
        static uint32 CurrentMountSpeed(const Unit* unit);
    private:
        bool MountWithBestMount(const bool canFly = false);

        bool Mount();
        bool UnMount() const;
    };
}
