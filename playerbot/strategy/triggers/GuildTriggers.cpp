#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "GuildTriggers.h"


using namespace ai;


bool LeaveLargeGuildTrigger::IsActive()
{
	if (!bot->GetGuildId())
		return false;

	if (ai->IsRealPlayer())
		return false;

	if (ai->IsAlt())
		return false;

	GuilderType type = ai->GetGuilderType();

	Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());

	Player* leader = sObjectMgr.GetPlayer(guild->GetLeaderGuid());

	//Only leave the guild if we know the leader is not a real player.
	if (!leader || !leader->GetPlayerbotAI() || leader->GetPlayerbotAI()->IsRealPlayer())
		return false;

	if (type == GuilderType::SOLO && guild->GetLeaderGuid() != bot->GetObjectGuid())
		return true;

	uint32 members = guild->GetMemberSize();
	uint32 maxMembers = uint8(type);

	return members > maxMembers;
}

