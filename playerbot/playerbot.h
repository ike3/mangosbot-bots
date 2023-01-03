#pragma once

#include "Spell.h"
#include "WorldPacket.h"
#include "LootMgr.h"
#include "GossipDef.h"
#include "Chat.h"
#include "Common.h"
#include "World.h"
#include "SpellMgr.h"
#include "ObjectMgr.h"
#include "Unit.h"
#include "SharedDefines.h"
#include "MotionMaster.h"
#include "SpellAuras.h"
#include "Guild.h"

#include "playerbotDefs.h"
#include "PlayerbotAIAware.h"
#include "PlayerbotMgr.h"
#include "RandomPlayerbotMgr.h"
#include "ChatHelper.h"
#include "PlayerbotAI.h"
#include "PlayerbotDbStore.h"

#define MANGOSBOT_VERSION 2

std::vector<std::string> split(std::string const& s, char delim);
void split(std::vector<std::string>& dest, std::string const& str, char const* delim);
#ifndef WIN32
int strcmpi(std::string s1, std::string s2);
#endif
