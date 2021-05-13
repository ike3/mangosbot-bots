#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "GenericTriggers.h"
#include "CureTriggers.h"
#include "../actions/WorldBuffAction.h""


using namespace ai;

bool NeedCureTrigger::IsActive() 
{
	Unit* target = GetTarget();
	return target && ai->HasAuraToDispel(target, dispelType);
}

Value<Unit*>* PartyMemberNeedCureTrigger::GetTargetValue()
{
	return context->GetValue<Unit*>("party member to dispel", dispelType);
}


bool NeedWorldBuffTrigger::IsActive()
{
    return !WorldBuffAction::NeedWorldBuffs(bot).empty();   
}
