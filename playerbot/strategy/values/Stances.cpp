#include "botpch.h"
#include "../../playerbot.h"
#include "Stances.h"

#include "../../ServerFacade.h"
#include "Arrow.h"

using namespace ai;

Unit* Stance::GetTarget()
{
    return AI_VALUE(Unit*, GetTargetName());
}

WorldLocation Stance::GetLocation()
{
    Unit* target = GetTarget();
    if (!target)
        return Formation::NullLocation;

    return GetLocationInternal();
}

WorldLocation Stance::GetNearLocation(float angle, float distance)
{
    Unit* target = GetTarget();

    float x = target->GetPositionX() + cos(angle) * distance,
         y = target->GetPositionY()+ sin(angle) * distance,
         z = target->GetPositionZ();

    if (bot->IsWithinLOS(x, y, z))
        return WorldLocation(bot->GetMapId(), x, y, z);

    return Formation::NullLocation;
}

WorldLocation MoveStance::GetLocationInternal()
{
    Unit* target = GetTarget();
    float distance = sPlayerbotAIConfig.meleeDistance;

    float angle = GetAngle();
    return GetNearLocation(angle, distance);
}


namespace ai
{

    class NearStance : public MoveStance
    {
    public:
        NearStance(PlayerbotAI* ai) : MoveStance(ai, "near") {}

        virtual float GetAngle()
        {
            return GetFollowAngle();
        }
    };

    class TankStance : public MoveStance
    {
    public:
        TankStance(PlayerbotAI* ai) : MoveStance(ai, "tank") {}

        virtual float GetAngle()
        {
            Unit* target = GetTarget();
            return bot->GetAngle(target);
        }
    };

    class BehindStance : public MoveStance
    {
    public:
        BehindStance(PlayerbotAI* ai) : MoveStance(ai, "behind") {}

        virtual float GetAngle()
        {
            Unit* target = GetTarget();
            return GetFollowAngle() / 3 + target->GetOrientation() + M_PI;
        }
    };
};

StanceValue::StanceValue(PlayerbotAI* ai) : ManualSetValue<Stance*>(ai, new NearStance(ai), "stance")
{
}

string StanceValue::Save()
{
    return value ? value->getName() : "?";
}

bool StanceValue::Load(string name)
{
    if (name == "behind")
    {
        if (value) delete value;
        value = new BehindStance(ai);
    }
    else if (name == "near" || name == "default")
    {
        if (value) delete value;
        value = new NearStance(ai);
    }
    else if (name == "tank")
    {
        if (value) delete value;
        value = new TankStance(ai);
    }
    else return false;

    return true;
}


bool SetStanceAction::Execute(Event event)
{
    string stance = event.getParam();

    StanceValue* value = (StanceValue*)context->GetValue<Stance*>("stance");
    if (stance == "?" || stance.empty())
    {
        ostringstream str; str << "Stance: |cff00ff00" << value->Get()->getName();
        ai->TellMaster(str);
        return true;
    }

    if (stance == "show")
    {
        WorldLocation loc = value->Get()->GetLocation();
        if (!Formation::IsNullLocation(loc))
            ai->Ping(loc.coord_x, loc.coord_y);
    }

    if (!value->Load(stance))
    {
        ostringstream str; str << "Invalid stance: |cffff0000" << stance;
        ai->TellMaster(str);
        ai->TellMaster("Please set to any of:|cffffffff near (default), tank, behind");
        return false;
    }

    ostringstream str; str << "Stance set to: " << stance;
    ai->TellMaster(str);
    return true;
}
