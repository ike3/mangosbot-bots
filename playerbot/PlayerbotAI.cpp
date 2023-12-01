#include "../botpch.h"
#include "PlayerbotMgr.h"
#include "playerbot.h"
#include <stdarg.h>
#include <iomanip>

#include "AiFactory.h"

#include "MotionGenerators/MovementGenerator.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "strategy/values/LastMovementValue.h"
#include "strategy/actions/LogLevelAction.h"
#include "strategy/actions/SayAction.h"
#include "strategy/actions/EmoteAction.h"
#include "strategy/values/LastSpellCastValue.h"
#include "LootObjectStack.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotAI.h"
#include "PlayerbotFactory.h"
#include "PlayerbotSecurity.h"
#include "Group.h"
#include "Pet.h"
#include "SpellAuras.h"
#include "../ahbot/AhBot.h"
#include "GuildTaskMgr.h"
#include "PlayerbotDbStore.h"
#include "strategy/values/PositionValue.h"
#include "ServerFacade.h"
#include "TravelMgr.h"
#include "MoveSplineInitArgs.h"
#include "ChatHelper.h"
#include "strategy/values/BudgetValues.h"
#include "Social/SocialMgr.h"
#include "PlayerbotTextMgr.h"
#include "RandomItemMgr.h"
#include "strategy/ItemVisitors.h"
#include "strategy/values/LootValues.h"
#include "Entities/Transports.h"
#ifdef MANGOSBOT_TWO
#include "Entities/Vehicle.h"
#endif

#ifdef BUILD_ELUNA
#include "LuaEngine/LuaEngine.h"
#endif

using namespace ai;
using namespace std;

vector<string>& split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);
char * strstri (string str1, string str2);
uint64 extractGuid(WorldPacket& packet);
std::string &trim(std::string &s);

set<string> PlayerbotAI::unsecuredCommands;

uint32 PlayerbotChatHandler::extractQuestId(string str)
{
    char* source = (char*)str.c_str();
    char* cId = ExtractKeyFromLink(&source,"Hquest");
    return cId ? atol(cId) : 0;
}

void PacketHandlingHelper::AddHandler(uint16 opcode, string handler, bool shouldDelay)
{
    handlers[opcode] = handler;
    delay[opcode] = shouldDelay;
}

void PacketHandlingHelper::Handle(ExternalEventHelper &helper)
{
    if (!m_botPacketMutex.try_lock()) //Packets do not have to be handled now. Handle them later.
        return;

    stack<WorldPacket> delayed;

    while (!queue.empty())
    {
        if (!helper.HandlePacket(handlers, queue.top()))
            if(delay[queue.top().GetOpcode()])
                delayed.push(queue.top());
        queue.pop();
    }

    queue = delayed;

    m_botPacketMutex.unlock();
}

void PacketHandlingHelper::AddPacket(const WorldPacket& packet)
{
    if (packet.empty() && packet.GetOpcode() != MSG_RAID_READY_CHECK)
        return;

    m_botPacketMutex.lock(); //We are going to add packets. Stop any new handling and add them.

	if (handlers.find(packet.GetOpcode()) != handlers.end())
        queue.push(WorldPacket(packet));

    m_botPacketMutex.unlock();
}


PlayerbotAI::PlayerbotAI() : PlayerbotAIBase(), bot(NULL), aiObjectContext(NULL),
    currentEngine(NULL), chatHelper(this), chatFilter(this), accountId(0), security(NULL), master(NULL), currentState(BotState::BOT_STATE_NON_COMBAT), faceTargetUpdateDelay(0)
{
    for (uint8 i = 0 ; i < (uint8)BotState::BOT_STATE_ALL; i++)
        engines[i] = NULL;

    for (int i = 0; i < MAX_ACTIVITY_TYPE; i++)
    {
        allowActiveCheckTimer[i] = time(nullptr);
        allowActive[i] = false;
    }
}

PlayerbotAI::PlayerbotAI(Player* bot) :
    PlayerbotAIBase(), chatHelper(this), chatFilter(this), security(bot), master(NULL), faceTargetUpdateDelay(0)
{
	this->bot = bot;    
    if (!bot->isTaxiCheater() && HasCheat(BotCheatMask::taxi))
        bot->SetTaxiCheater(true);

    for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
        engines[i] = NULL;

    for (int i = 0; i < MAX_ACTIVITY_TYPE; i++)
    {
        allowActiveCheckTimer[i] = time(nullptr);
        allowActive[i] = false;
    }

	accountId = sObjectMgr.GetPlayerAccountIdByGUID(bot->GetObjectGuid());

    aiObjectContext = AiFactory::createAiObjectContext(bot, this);

    UpdateTalentSpec();

    engines[(uint8)BotState::BOT_STATE_COMBAT] = AiFactory::createCombatEngine(bot, this, aiObjectContext);
    engines[(uint8)BotState::BOT_STATE_NON_COMBAT] = AiFactory::createNonCombatEngine(bot, this, aiObjectContext);
    engines[(uint8)BotState::BOT_STATE_DEAD] = AiFactory::createDeadEngine(bot, this, aiObjectContext);
    engines[(uint8)BotState::BOT_STATE_REACTION] = reactionEngine = AiFactory::createReactionEngine(bot, this, aiObjectContext);

    for (uint8 e = 0; e < (uint8)BotState::BOT_STATE_ALL; e++)
    {
        engines[e]->initMode = false;
        engines[e]->Init();
    }

    currentEngine = engines[(uint8)BotState::BOT_STATE_NON_COMBAT];
    currentState = BotState::BOT_STATE_NON_COMBAT;

    masterIncomingPacketHandlers.AddHandler(CMSG_GAMEOBJ_USE, "use game object");
    masterIncomingPacketHandlers.AddHandler(CMSG_AREATRIGGER, "area trigger");
    masterIncomingPacketHandlers.AddHandler(CMSG_GAMEOBJ_USE, "use game object");
    masterIncomingPacketHandlers.AddHandler(CMSG_LOOT_ROLL, "loot roll", true);
    masterIncomingPacketHandlers.AddHandler(CMSG_GOSSIP_HELLO, "gossip hello");
    masterIncomingPacketHandlers.AddHandler(CMSG_QUESTGIVER_HELLO, "gossip hello");
    masterIncomingPacketHandlers.AddHandler(CMSG_QUESTGIVER_COMPLETE_QUEST, "complete quest");
    masterIncomingPacketHandlers.AddHandler(CMSG_QUESTGIVER_ACCEPT_QUEST, "accept quest");
    masterIncomingPacketHandlers.AddHandler(CMSG_ACTIVATETAXI, "activate taxi");
    masterIncomingPacketHandlers.AddHandler(CMSG_ACTIVATETAXIEXPRESS, "activate taxi");
    masterIncomingPacketHandlers.AddHandler(CMSG_TAXICLEARALLNODES, "taxi done");
    masterIncomingPacketHandlers.AddHandler(CMSG_TAXICLEARNODE, "taxi done");
    masterIncomingPacketHandlers.AddHandler(CMSG_GROUP_UNINVITE, "uninvite");
    masterIncomingPacketHandlers.AddHandler(CMSG_GROUP_UNINVITE_GUID, "uninvite guid");
    masterIncomingPacketHandlers.AddHandler(CMSG_PUSHQUESTTOPARTY, "quest share");
    masterIncomingPacketHandlers.AddHandler(CMSG_CAST_SPELL, "see spell");
    masterIncomingPacketHandlers.AddHandler(CMSG_REPOP_REQUEST, "release spirit");
    masterIncomingPacketHandlers.AddHandler(CMSG_RECLAIM_CORPSE, "revive from corpse");
    

#ifdef MANGOSBOT_TWO
    masterIncomingPacketHandlers.AddHandler(CMSG_LFG_TELEPORT, "lfg teleport");
#endif
    
    botOutgoingPacketHandlers.AddHandler(SMSG_PETITION_SHOW_SIGNATURES, "petition offer");
    botOutgoingPacketHandlers.AddHandler(SMSG_BATTLEFIELD_STATUS, "bg status");
    botOutgoingPacketHandlers.AddHandler(SMSG_GROUP_INVITE, "group invite");
    botOutgoingPacketHandlers.AddHandler(SMSG_GUILD_INVITE, "guild accept");
    botOutgoingPacketHandlers.AddHandler(BUY_ERR_NOT_ENOUGHT_MONEY, "not enough money");
    botOutgoingPacketHandlers.AddHandler(BUY_ERR_REPUTATION_REQUIRE, "not enough reputation");
    botOutgoingPacketHandlers.AddHandler(SMSG_GROUP_SET_LEADER, "group set leader");
    botOutgoingPacketHandlers.AddHandler(SMSG_FORCE_RUN_SPEED_CHANGE, "check mount state");
    botOutgoingPacketHandlers.AddHandler(SMSG_RESURRECT_REQUEST, "resurrect request");
    botOutgoingPacketHandlers.AddHandler(SMSG_INVENTORY_CHANGE_FAILURE, "cannot equip");
    botOutgoingPacketHandlers.AddHandler(SMSG_TRADE_STATUS, "trade status");
    botOutgoingPacketHandlers.AddHandler(SMSG_LOOT_RESPONSE, "loot response", true);
    botOutgoingPacketHandlers.AddHandler(SMSG_QUESTUPDATE_ADD_KILL, "quest objective completed", true);
    botOutgoingPacketHandlers.AddHandler(SMSG_ITEM_PUSH_RESULT, "item push result", true);
    botOutgoingPacketHandlers.AddHandler(SMSG_PARTY_COMMAND_RESULT, "party command");
    botOutgoingPacketHandlers.AddHandler(SMSG_LEVELUP_INFO, "levelup", true);
    botOutgoingPacketHandlers.AddHandler(SMSG_LOG_XPGAIN, "xpgain", true);
    botOutgoingPacketHandlers.AddHandler(SMSG_TEXT_EMOTE, "receive text emote");
    botOutgoingPacketHandlers.AddHandler(SMSG_EMOTE, "receive emote");
    botOutgoingPacketHandlers.AddHandler(SMSG_LOOT_START_ROLL, "loot start roll", true);
    botOutgoingPacketHandlers.AddHandler(SMSG_SUMMON_REQUEST, "summon request");
    botOutgoingPacketHandlers.AddHandler(MSG_RAID_READY_CHECK, "ready check");

    
#ifndef MANGOSBOT_ZERO
    botOutgoingPacketHandlers.AddHandler(SMSG_ARENA_TEAM_INVITE, "arena team invite");
#endif
#ifdef MANGOSBOT_TWO
    botOutgoingPacketHandlers.AddHandler(SMSG_LFG_ROLE_CHECK_UPDATE, "lfg role check");
    botOutgoingPacketHandlers.AddHandler(SMSG_LFG_PROPOSAL_UPDATE, "lfg proposal");
#endif
    
    
#ifdef MANGOS
    botOutgoingPacketHandlers.AddHandler(SMSG_CAST_FAILED, "cast failed");
#endif
#ifdef CMANGOS
    botOutgoingPacketHandlers.AddHandler(SMSG_CAST_RESULT, "cast failed");
#endif
    botOutgoingPacketHandlers.AddHandler(SMSG_DUEL_REQUESTED, "duel requested");
    botOutgoingPacketHandlers.AddHandler(SMSG_INVENTORY_CHANGE_FAILURE, "inventory change failure");

    masterOutgoingPacketHandlers.AddHandler(SMSG_PARTY_COMMAND_RESULT, "party command");
    masterOutgoingPacketHandlers.AddHandler(MSG_RAID_READY_CHECK_FINISHED, "ready check finished");
}

PlayerbotAI::~PlayerbotAI()
{
    for (uint8 i = 0 ; i < (uint8)BotState::BOT_STATE_ALL; i++)
    {
        if (engines[i])
            delete engines[i];
    }

    if (aiObjectContext)
        delete aiObjectContext;
}

void PlayerbotAI::UpdateAI(uint32 elapsed, bool minimal)
{
    string mapString = WorldPosition(bot).isOverworld() ? to_string(bot->GetMapId()) : "I";
    PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_TOTAL, "PlayerbotAI::UpdateAI " + mapString);
    if(aiInternalUpdateDelay > elapsed)
    {
        aiInternalUpdateDelay -= elapsed;
    }
    else
    {
        aiInternalUpdateDelay = 0;
        isWaiting = false;
    }

    // cancel logout in combat
    if (bot->IsStunnedByLogout() || bot->GetSession()->isLogingOut())
    {
        if (sServerFacade.IsInCombat(bot) || (master && sServerFacade.IsInCombat(master) && sServerFacade.GetDistance2d(bot, master) < 30.0f))
        {
            WorldPacket p;
            bot->GetSession()->HandleLogoutCancelOpcode(p);
            TellPlayer(GetMaster(), BOT_TEXT("logout_cancel"));
        }
    }

    // Leontiesh - fix movement desync
    bool botMoving = false;
    if (!bot->IsStopped() || bot->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
        botMoving = true;
    if (!bot->GetMotionMaster()->empty())
        if (MovementGenerator* movgen = bot->GetMotionMaster()->top())
            botMoving = true;

    if (botMoving && !bot->IsBeingTeleported() && bot->IsInWorld())
    {
        isMoving = true;

        // release loot if moving
        if (!bot->GetLootGuid().IsEmpty())
            if (Loot* loot = sLootMgr.GetLoot(bot, bot->GetLootGuid()))
                loot->Release(bot);
    }
    else if (isMoving)
    {
        if (!bot->IsTaxiFlying())
            StopMoving();

        isMoving = false;
    }

    // wake up if in combat
    bool isCasting = bot->IsNonMeleeSpellCasted(true);
    if (sServerFacade.IsInCombat(bot))
    {
        if (!inCombat && !isCasting && !isWaiting)
        {
            ResetAIInternalUpdateDelay();
        }
        else if (!AllowActivity())
        {
            if (AllowActivity(ALL_ACTIVITY, true))
                ResetAIInternalUpdateDelay();
        }

        inCombat = true;
    }
    else
    {
        if (inCombat && !isCasting && !isWaiting)
            ResetAIInternalUpdateDelay();

        inCombat = false;
    }

    // force stop if moving but should not
#ifndef MANGOSBOT_TWO
    if (!bot->IsStopped() && !CanMove() && !bot->m_movementInfo.HasMovementFlag(MOVEFLAG_JUMPING) && !bot->IsTaxiFlying() && !bot->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING) && !bot->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED))
#else
    if (!bot->IsStopped() && !CanMove() && !bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLING) && !bot->IsTaxiFlying() && !bot->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING) && !bot->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED))
#endif
    {
        StopMoving();
    }

    // cheat options
    if (bot->IsAlive() && ((uint32)GetCheat() > 0 || (uint32)sPlayerbotAIConfig.botCheatMask > 0))
    {
        if (HasCheat(BotCheatMask::health))
            bot->SetHealthPercent(100);
        if (HasCheat(BotCheatMask::mana) && bot->GetPowerType() == POWER_MANA)
            bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));
        if (HasCheat(BotCheatMask::power) && bot->GetPowerType() != POWER_MANA)
            bot->SetPower(bot->GetPowerType(), bot->GetMaxPower(bot->GetPowerType()));
        if (HasCheat(BotCheatMask::repair))
#ifdef MANGOSBOT_ZERO
            bot->DurabilityRepairAll(false, 0);
#else
            bot->DurabilityRepairAll(false, 0, false);
#endif
        if (HasCheat(BotCheatMask::cooldown))
            bot->RemoveAllCooldowns();
        if (HasCheat(BotCheatMask::movespeed))
        {
            bot->UpdateSpeed(MOVE_WALK, true, 10);
            bot->UpdateSpeed(MOVE_RUN, true, 10);
            bot->UpdateSpeed(MOVE_SWIM, true, 10);
        }
        if (HasCheat(BotCheatMask::breath))
        {
            bot->SetWaterBreathingIntervalMultiplier(0);
        }
        if (HasCheat(BotCheatMask::item) && (bot->getClass() == CLASS_HUNTER || bot->getClass() == CLASS_ROGUE || bot->getClass() == CLASS_WARRIOR))
        {
            uint32 itemId = bot->GetUInt32Value(PLAYER_AMMO_ID);
            if (itemId && bot->GetItemCount(itemId))
            {
                AiObjectContext* context = aiObjectContext;

                list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", this->chatHelper.formatQItem(itemId));

                for (auto item : items)
                {
                    if (bot->getClass() == CLASS_HUNTER && item->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_THROWN) //Do not replenish thrown weapons for hunters.
                        break;

                    item->SetCount(item->GetProto()->GetMaxStackSize());
                    break;
                }
            }
            else
            {
                PlayerbotFactory(bot, bot->GetLevel(), 0).InitAmmo();
            }
        }
    }

    if (master && IsSafe(master) && bot->GetDistance(master) < INTERACTION_DISTANCE * 2.5 && master->GetTransport() != bot->GetTransport())
    {
        bot->StopMoving();
        if (master->GetTransport() && WorldPosition(bot).isOnTransport(master->GetTransport()))
            master->GetTransport()->AddPassenger(bot);
        else if (bot->GetTransport())
        {
            WorldPosition botPos(bot);
            bot->GetTransport()->RemovePassenger(bot);
            bot->NearTeleportTo(bot->m_movementInfo.pos.x, bot->m_movementInfo.pos.y, bot->m_movementInfo.pos.z, bot->m_movementInfo.pos.o);
            MANGOS_ASSERT(botPos.fDist(bot) < 500.0f);
        }
    }
    else if (!HasRealPlayerMaster() && !bot->IsBeingTeleported() && bot->GetTransport() && bot->GetMapId() == bot->GetTransport()->GetMapId() && !WorldPosition(bot).isOnTransport(bot->GetTransport()) && !isMovingToTransport)
    {
        if (HasStrategy("debug move", BotState::BOT_STATE_NON_COMBAT))
        {
            TellPlayer(GetMaster(), "Jumping off " + string(bot->GetTransport()->GetName()));
        }

        WorldPosition botPos(bot);
        bot->GetTransport()->RemovePassenger(bot);
        bot->NearTeleportTo(bot->m_movementInfo.pos.x, bot->m_movementInfo.pos.y, bot->m_movementInfo.pos.z, bot->m_movementInfo.pos.o);
        MANGOS_ASSERT(botPos.fDist(bot) < 500.0f);
        bot->StopMoving();
    }

    // Update facing
    UpdateFaceTarget(elapsed, minimal);

    bool doMinimalReaction = minimal || !AllowActivity(REACT_ACTIVITY);

#ifdef PLAYERBOT_ELUNA
    // used by eluna    
    sEluna->OnUpdateAI(this, bot->GetName());
#endif

    // Only update the internal ai when no reaction is running and the internal ai can be updated
    if(!UpdateAIReaction(elapsed, doMinimalReaction, bot->IsTaxiFlying()) && CanUpdateAIInternal())
    {      
        // Update the delay with the spell cast time
        Spell* currentSpell = bot->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (currentSpell && (currentSpell->getState() == SPELL_STATE_CASTING) && (currentSpell->GetCastedTime() > 0U))
        {
            SetAIInternalUpdateDelay(currentSpell->GetCastedTime() + sPlayerbotAIConfig.reactDelay + sWorld.GetAverageDiff());
    
            // Cancel the update if the new delay increased
            if (!CanUpdateAIInternal())
            {
                if (pmo) pmo->finish();
                return;
            }
        }

        UpdateAIInternal(elapsed, minimal);

        bool min = minimal;
        // test fix lags because of BG
        if (!inCombat)
            min = true;

        if (bot && HasRealPlayerMaster())
            min = false;

        YieldAIInternalThread(min);
    }
    if (pmo) pmo->finish();
}

bool PlayerbotAI::UpdateAIReaction(uint32 elapsed, bool minimal, bool isStunned)
{
    bool reactionFound;
    string mapString = WorldPosition(bot).isOverworld() ? to_string(bot->GetMapId()) : "I";
    PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_TOTAL, "PlayerbotAI::UpdateAIReaction " + mapString);
    const bool reactionInProgress = reactionEngine->Update(elapsed, minimal, isStunned, reactionFound);
    if (pmo) pmo->finish();

    if(reactionFound)
    {
        // If new reaction found force stop current actions (if required)
        const Reaction* reaction = reactionEngine->GetReaction();
        if(reaction)
        {
            if(reaction->ShouldInterruptCast())
            {
                InterruptSpell();
            }

            if (reaction->ShouldInterruptMovement())
            {
                StopMoving();
            }
        }
    }

    return reactionInProgress;
}

void PlayerbotAI::UpdateFaceTarget(uint32 elapsed, bool minimal)
{
    faceTargetUpdateDelay = faceTargetUpdateDelay > elapsed ? faceTargetUpdateDelay - elapsed : 0U;
    if (faceTargetUpdateDelay <= 0U)
    {
        // Only update the target facing when in combat
        if (IsStateActive(BotState::BOT_STATE_COMBAT))
        {
            // Don't update facing if bot is moving
            if (bot->IsStopped())
            {
                AiObjectContext* context = GetAiObjectContext();
                Unit* target = AI_VALUE(Unit*, "current target");
                if(target)
                {
                    // Do not update the facing while pulling
                    Unit* pullTarget = AI_VALUE(Unit*, "pull target");
                    if (pullTarget == nullptr)
                    {
                        if (!AI_VALUE2(bool, "facing", "current target"))
                        {
                            if (!sServerFacade.UnitIsDead(bot) &&
                                !sServerFacade.IsFrozen(bot) &&
                                !sServerFacade.IsCharmed(bot) &&
                                !bot->IsPolymorphed() &&
                                !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST) &&
                                !bot->IsBeingTeleported() &&
                                !bot->HasAuraType(SPELL_AURA_MOD_CONFUSE) &&
                                !bot->HasAuraType(SPELL_AURA_MOD_STUN) &&
                                !bot->IsTaxiFlying() &&
                                !bot->hasUnitState(UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL))
                            {
                                sServerFacade.SetFacingTo(bot, target);
                            }
                        }
                    }
                }
            }
        }

        faceTargetUpdateDelay = minimal ? sPlayerbotAIConfig.reactDelay * 10 : sPlayerbotAIConfig.reactDelay * 5;
    }
}

void PlayerbotAI::SetActionDuration(const Action* action)
{
    if (action)
    {
        if(action->IsReaction())
        {
            reactionEngine->SetReactionDuration(action);
        }
        else
        {
            SetAIInternalUpdateDelay(action->GetDuration());
        }
    }
}

void PlayerbotAI::SetActionDuration(uint32 duration)
{
    SetAIInternalUpdateDelay(duration);
}

const Action* PlayerbotAI::GetLastExecutedAction(BotState state) const
{
    const Engine* engine = engines[(uint8)state];
    if(engine)
    {
        return engine->GetLastExecutedAction();
    }

    return nullptr;
}

bool PlayerbotAI::IsImmuneToSpell(uint32 spellId) const
{
    for (list<uint32>::iterator i = sPlayerbotAIConfig.immuneSpellIds.begin(); i != sPlayerbotAIConfig.immuneSpellIds.end(); ++i)
    {
        if (spellId == (*i))
        {
            return true;
        }
    }

    return false;
}

bool PlayerbotAI::IsInPve()
{
    return !IsInPvp() && !IsInRaid();
}

bool PlayerbotAI::IsInPvp()
{
    if (IsSafe(bot))
    {
        const bool inDuel = bot->duel && bot->duel->opponent;
        if (!inDuel)
        {
            const bool inBattleground = bot->InBattleGround();
            bool inArena = false;
#ifndef MANGOSBOT_ZERO
            inArena = bot->InArena();
#endif
            if (!inBattleground && !inArena)
            {
                AiObjectContext* context = aiObjectContext;
                const bool isPlayerNear = AI_VALUE(bool, "has enemy player targets");
                if (!isPlayerNear)
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool PlayerbotAI::IsInRaid()
{
    bool inRaidFight = false;
    if (IsSafe(bot))
    {
        const Map* map = bot->GetMap();
        if (map && (map->IsDungeon() || map->IsRaid()))
        {
            inRaidFight = true;
        }
        else if (!IsInPvp() && GetState() == BotState::BOT_STATE_COMBAT)
        {
            AiObjectContext* context = GetAiObjectContext();
            const std::list<ObjectGuid>& attackers = AI_VALUE(std::list<ObjectGuid>, "attackers");
            for (const ObjectGuid& attackerGuid : attackers)
            {
                Creature* creature = GetCreature(attackerGuid);
                if (creature)
                {
                    const CreatureInfo* creatureInfo = creature->GetCreatureInfo();
                    if (creatureInfo)
                    {
                        if (creatureInfo->Rank == CREATURE_ELITE_WORLDBOSS)
                        {
                            inRaidFight = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    return inRaidFight;
}

PlayerTalentSpec PlayerbotAI::GetTalentSpec()
{
    return aiObjectContext->GetValue<PlayerTalentSpec>("talent spec")->Get();
}

void PlayerbotAI::UpdateTalentSpec(PlayerTalentSpec spec)
{
    if(spec == PlayerTalentSpec::TALENT_SPEC_INVALID)
    {
        int talentsTab = 0;
        if(bot->GetLevel() < 10)
        {
            switch (bot->getClass())
            {
                case CLASS_MAGE:
                {
                    talentsTab = 1;
                    break;
                }

                case CLASS_PALADIN:
                {
                    talentsTab = 0;
                    break;
                }

                case CLASS_PRIEST:
                {
                    talentsTab = 1;
                    break;
                }
            }
        }
        else
        {
            talentsTab = AiFactory::GetPlayerSpecTab(bot);
        }

        spec = PlayerTalentSpec(((bot->getClass() * 3) - 2) + talentsTab);
    }

    aiObjectContext->GetValue<PlayerTalentSpec>("talent spec")->Set(spec);
}

bool PlayerbotAI::IsStateActive(BotState state) const
{
    return currentEngine == engines[(uint8)state];
}

time_t PlayerbotAI::GetCombatStartTime() const
{
    return aiObjectContext->GetValue<time_t>("combat start time")->Get();
}

void PlayerbotAI::OnCombatStarted()
{
    if(!IsStateActive(BotState::BOT_STATE_COMBAT))
    {
        aiObjectContext->GetValue<time_t>("combat start time")->Set(time(0));
        ChangeEngine(BotState::BOT_STATE_COMBAT);
    }
}

void PlayerbotAI::OnCombatEnded()
{
    if (!IsStateActive(BotState::BOT_STATE_NON_COMBAT))
    {
        aiObjectContext->GetValue<time_t>("combat start time")->Set(0);
        ChangeEngine(BotState::BOT_STATE_NON_COMBAT);
    }
}

void PlayerbotAI::OnDeath()
{
    if (!IsStateActive(BotState::BOT_STATE_DEAD) && !sServerFacade.IsAlive(bot))
    {
        StopMoving();

        //Death Count to prevent skeleton piles
        Player* master = GetMaster();
        if (!HasActivePlayerMaster() && !bot->InBattleGround())
        {
            uint32 dCount = aiObjectContext->GetValue<uint32>("death count")->Get();
            aiObjectContext->GetValue<uint32>("death count")->Set(++dCount);

            if (sPlayerbotAIConfig.hasLog("deaths.csv"))
            {
                WorldPosition botPos(bot);

                ostringstream out;
                out << sPlayerbotAIConfig.GetTimestampStr() << "+00,";
                out << bot->GetName() << ",";
                out << std::fixed << std::setprecision(2);

                out << to_string(bot->getRace()) << ",";
                out << to_string(bot->getClass()) << ",";
                float subLevel = ((float)bot->GetLevel() + ((float)bot->GetUInt32Value(PLAYER_XP) / (float)bot->GetUInt32Value(PLAYER_NEXT_LEVEL_XP)));

                out << subLevel << ",";

                botPos.printWKT(out);

                AiObjectContext* context = GetAiObjectContext();

                Unit* ctarget = AI_VALUE(Unit*, "current target");

                if (ctarget)
                {
                    out << "\"" << ctarget->GetName() << "\"," << ctarget->GetLevel() << "," << ctarget->GetHealthPercent() << ",";
                }
                else
                    out << "\"none\",0,100,";

                list<ObjectGuid> targets = AI_VALUE_LAZY(list<ObjectGuid>, "all targets");

                out << "\"";

                uint32 adds = 0;

                for (auto target : targets)
                {
                    if (!target.IsCreature())
                        continue;

                    Unit* unit = GetUnit(target);
                    if (!unit)
                        continue;

                    if (unit->GetVictim() != bot)
                        continue;

                    if (unit == ctarget)
                        continue;

                    out << unit->GetName() << "(" << unit->GetLevel() << ")";

                    adds++;
                }

                out << "\"," << to_string(adds);

                sPlayerbotAIConfig.log("deaths.csv", out.str().c_str());
            }
        }

        aiObjectContext->GetValue<Unit*>("current target")->Set(nullptr);
        aiObjectContext->GetValue<Unit*>("enemy player target")->Set(nullptr);
        aiObjectContext->GetValue<Unit*>("pull target")->Set(nullptr);
        aiObjectContext->GetValue<ObjectGuid>("attack target")->Set(ObjectGuid());
        aiObjectContext->GetValue<LootObject>("loot target")->Set(LootObject());
        aiObjectContext->GetValue<time_t>("combat start time")->Set(0);
        ChangeEngine(BotState::BOT_STATE_DEAD);
    }
}

void PlayerbotAI::OnResurrected()
{
    if (IsStateActive(BotState::BOT_STATE_DEAD) && sServerFacade.IsAlive(bot))
    {
        ChangeEngine(BotState::BOT_STATE_NON_COMBAT);
    }
}

void PlayerbotAI::HandleCommands()
{

    ExternalEventHelper helper(aiObjectContext);
    list<ChatCommandHolder> delayed;
    while (!chatCommands.empty())
    {
        ChatCommandHolder holder = chatCommands.front();
        time_t checkTime = holder.GetTime();
        if (checkTime && time(0) < checkTime)
        {
            delayed.push_back(holder);
            chatCommands.pop();
            continue;
        }
        
        string command = holder.GetCommand();
        Player* owner = holder.GetOwner();
        if (!helper.ParseChatCommand(command, owner) && holder.GetType() == CHAT_MSG_WHISPER)
        {
            //ostringstream out; out << "Unknown command " << command;
            //TellPlayer(out);
            //helper.ParseChatCommand("help");
        }
        
        chatCommands.pop();
    }

    for (list<ChatCommandHolder>::iterator i = delayed.begin(); i != delayed.end(); ++i)
    {
        chatCommands.push(*i);
    }
}

void PlayerbotAI::UpdateAIInternal(uint32 elapsed, bool minimal)
{
    if (bot->IsBeingTeleported() || !bot->IsInWorld())
        return;

    string mapString = WorldPosition(bot).isOverworld() ? to_string(bot->GetMapId()) : "I";
    PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_TOTAL, "PlayerbotAI::UpdateAIInternal " + mapString);

    ExternalEventHelper helper(aiObjectContext);

    // chat replies
    list<ChatQueuedReply> delayedResponses;
    while (!chatReplies.empty())
    {
        ChatQueuedReply holder = chatReplies.front();
        time_t checkTime = holder.m_time;
        if (checkTime && time(0) < checkTime)
        {
            delayedResponses.push_back(holder);
            chatReplies.pop();
            continue;
        }
        ChatReplyAction::ChatReplyDo(bot, holder.m_type, holder.m_guid1, holder.m_guid2, holder.m_msg, holder.m_chanName, holder.m_name);
        chatReplies.pop();
    }

    for (list<ChatQueuedReply>::iterator i = delayedResponses.begin(); i != delayedResponses.end(); ++i)
    {
        chatReplies.push(*i);
    }

    // logout if logout timer is ready or if instant logout is possible
    if (bot->IsStunnedByLogout() || bot->GetSession()->isLogingOut())
    {
        WorldSession* botWorldSessionPtr = bot->GetSession();
        bool logout = botWorldSessionPtr->ShouldLogOut(time(nullptr));
        if (!master || master->GetSession()->GetState() != WORLD_SESSION_STATE_READY)
            logout = true;

        if (bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || bot->IsTaxiFlying() ||
            botWorldSessionPtr->GetSecurity() >= (AccountTypes)sWorld.getConfig(CONFIG_UINT32_INSTANT_LOGOUT))
        {
            logout = true;
        }

        if (master && (master->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || master->IsTaxiFlying() ||
            (master->GetSession() && master->GetSession()->GetSecurity() >= (AccountTypes)sWorld.getConfig(CONFIG_UINT32_INSTANT_LOGOUT))))
        {
            logout = true;
        }

        if (logout && !bot->GetSession()->ShouldLogOut(time(nullptr)))
        {
            if (master && master->GetPlayerbotMgr())
            {
                master->GetPlayerbotMgr()->LogoutPlayerBot(bot->GetObjectGuid().GetRawValue());
            }
            else
            {
                sRandomPlayerbotMgr.LogoutPlayerBot(bot->GetObjectGuid().GetRawValue());
            }
            return;
        }

        SetAIInternalUpdateDelay(sPlayerbotAIConfig.reactDelay);
        return;
    }

    botOutgoingPacketHandlers.Handle(helper);
    masterIncomingPacketHandlers.Handle(helper);
    masterOutgoingPacketHandlers.Handle(helper);

	DoNextAction(minimal);
	if (pmo) pmo->finish();
}

void PlayerbotAI::HandleTeleportAck()
{
    if (IsRealPlayer())
        return;

    StopMoving();

	if (bot->IsBeingTeleportedNear())
	{
        if (!bot->GetMotionMaster()->empty())
            if (MovementGenerator* movgen = bot->GetMotionMaster()->top())
                movgen->Interrupt(*bot);

       /* WorldLocation dest = bot->GetTeleportDest();
        bot->Relocate(dest.coord_x, dest.coord_y, dest.coord_z, dest.orientation);*/

		WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
#ifdef MANGOSBOT_TWO
        p << bot->GetObjectGuid().WriteAsPacked();
#else
        p << bot->GetObjectGuid();
#endif
		p << (uint32) 0; // supposed to be flags? not used currently
		p << (uint32) time(0); // time - not currently used
        bot->GetSession()->HandleMoveTeleportAckOpcode(p);

        // add delay to simulate teleport delay
        SetAIInternalUpdateDelay(urand(1000, 2000));
	}
	else if (bot->IsBeingTeleportedFar())
	{
        bot->GetSession()->HandleMoveWorldportAckOpcode();

        // add delay to simulate teleport delay
        SetAIInternalUpdateDelay(urand(2000, 5000));
	}

    Reset();
}

void PlayerbotAI::Reset(bool full)
{
    if (bot->IsTaxiFlying())
        return;

    currentEngine = engines[(uint8)BotState::BOT_STATE_NON_COMBAT];
    currentState = BotState::BOT_STATE_NON_COMBAT;
    ResetAIInternalUpdateDelay();
    reactionEngine->Reset();
    whispers.clear();

    PullStrategy* strategy = PullStrategy::Get(this);
    if (strategy)         
        strategy->OnPullEnded();

    aiObjectContext->GetValue<Unit*>("old target")->Set(nullptr);
    aiObjectContext->GetValue<Unit*>("current target")->Set(nullptr);
    aiObjectContext->GetValue<Unit*>("pull target")->Set(nullptr);
    aiObjectContext->GetValue<ObjectGuid>("attack target")->Set(ObjectGuid());
    aiObjectContext->GetValue<GuidPosition>("rpg target")->Set(GuidPosition());
    aiObjectContext->GetValue<LootObject>("loot target")->Set(LootObject());
    aiObjectContext->GetValue<uint32>("lfg proposal")->Set(0);
    aiObjectContext->GetValue<time_t>("combat start time")->Set(0);
    bot->SetSelectionGuid(ObjectGuid());

    LastSpellCast & lastSpell = aiObjectContext->GetValue<LastSpellCast& >("last spell cast")->Get();
    lastSpell.Reset();

    if (bot->GetTradeData())
        bot->TradeCancel(true);

    if (full)
    {
        aiObjectContext->GetValue<LastMovement& >("last movement")->Get().Set(NULL);
        aiObjectContext->GetValue<LastMovement& >("last area trigger")->Get().Set(NULL);
        aiObjectContext->GetValue<LastMovement& >("last taxi")->Get().Set(NULL);

        aiObjectContext->GetValue<TravelTarget* >("travel target")->Get()->setTarget(sTravelMgr.nullTravelDestination, sTravelMgr.nullWorldPosition, true);
        aiObjectContext->GetValue<TravelTarget* >("travel target")->Get()->setStatus(TravelStatus::TRAVEL_STATUS_EXPIRED);
        aiObjectContext->GetValue<TravelTarget* >("travel target")->Get()->setExpireIn(1000);

        InterruptSpell();

        StopMoving();

        WorldSession* botWorldSessionPtr = bot->GetSession();
        bool logout = botWorldSessionPtr->ShouldLogOut(time(nullptr));

        // cancel logout
        if (!logout && (bot->IsStunnedByLogout() || bot->GetSession()->isLogingOut()))
        {
            WorldPacket p;
            bot->GetSession()->HandleLogoutCancelOpcode(p);
            TellPlayer(GetMaster(), BOT_TEXT("logout_cancel"));
        }
    }
    
    aiObjectContext->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get().clear();

    if (bot->IsTaxiFlying())
    {
#ifdef MANGOS
        bot->m_taxi.ClearTaxiDestinations();
#endif
        bot->OnTaxiFlightEject(true);
    }

    if (full)
    {
        for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
        {
            engines[i]->Init();
        }
    }
}

map<string,ChatMsg> chatMap;

bool PlayerbotAI::IsAllowedCommand(string text)
{
    if (unsecuredCommands.empty())
    {
        unsecuredCommands.insert("who");
        unsecuredCommands.insert("wts");
        unsecuredCommands.insert("sendmail");
        unsecuredCommands.insert("invite");
        unsecuredCommands.insert("leave");
        unsecuredCommands.insert("join");
        unsecuredCommands.insert("lfg");
        unsecuredCommands.insert("guild invite");
        unsecuredCommands.insert("guild leave");
    }

    for (set<string>::iterator i = unsecuredCommands.begin(); i != unsecuredCommands.end(); ++i)
    {
        if (text.find(*i) == 0)
        {
            return true;
        }
    }

    return false;
}

void PlayerbotAI::HandleCommand(uint32 type, const string& text, Player& fromPlayer, const uint32 lang)
{
    string filtered = text;

    if (!IsAllowedCommand(filtered) && !GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE, type != CHAT_MSG_WHISPER, &fromPlayer))
        return;

    if (type == CHAT_MSG_ADDON)
        return;

    if (filtered.find("BOT\t") == 0) //Mangosbot has BOT prefix so we remove that.
        filtered = filtered.substr(4);
    else if (lang == LANG_ADDON) //Other addon messages should not command bots.
        return;

    if (type == CHAT_MSG_SYSTEM)
        return;

    if (filtered.find(sPlayerbotAIConfig.commandSeparator) != string::npos)
    {
        vector<string> commands;
        split(commands, filtered, sPlayerbotAIConfig.commandSeparator.c_str());
        for (vector<string>::iterator i = commands.begin(); i != commands.end(); ++i)
        {
            HandleCommand(type, *i, fromPlayer);
        }
        return;
    }

    if (!sPlayerbotAIConfig.commandPrefix.empty())
    {
        if (filtered.find(sPlayerbotAIConfig.commandPrefix) != 0)
            return;

        filtered = filtered.substr(sPlayerbotAIConfig.commandPrefix.size());
    }

    if (chatMap.empty())
    {
        chatMap["#w "] = CHAT_MSG_WHISPER;
        chatMap["#p "] = CHAT_MSG_PARTY;
        chatMap["#r "] = CHAT_MSG_RAID;
        chatMap["#a "] = CHAT_MSG_ADDON;
        chatMap["#g "] = CHAT_MSG_GUILD;
    }
    currentChat = pair<ChatMsg, time_t>(CHAT_MSG_WHISPER, 0);
    for (map<string,ChatMsg>::iterator i = chatMap.begin(); i != chatMap.end(); ++i)
    {
        if (filtered.find(i->first) == 0)
        {
            filtered = filtered.substr(3);
            currentChat = pair<ChatMsg, time_t>(i->second, time(0) + 3);
            break;
        }
    }

    filtered = chatFilter.Filter(trim((string&)filtered));
    if (filtered.empty())
        return;

    if (filtered.substr(0, 6) == "debug ")
    {
        string response = HandleRemoteCommand(filtered.substr(6));
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, CHAT_MSG_ADDON, response.c_str(), LANG_ADDON,
                CHAT_TAG_NONE, bot->GetObjectGuid(), bot->GetName());
        sServerFacade.SendPacket(&fromPlayer, data);
        return;
    }

    if (!IsAllowedCommand(filtered) && !GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, type != CHAT_MSG_WHISPER, &fromPlayer))
        return;

    if (type == CHAT_MSG_RAID_WARNING && filtered.find(bot->GetName()) != string::npos && filtered.find("award") == string::npos)
    {
        ChatCommandHolder cmd("warning", &fromPlayer, type);
        chatCommands.push(cmd);
        return;
    }

    if ((filtered.size() > 2 && filtered.substr(0, 2) == "d ") || (filtered.size() > 3 && filtered.substr(0, 3) == "do "))
    {
        std::string action = filtered.substr(filtered.find(" ") + 1);
        DoSpecificAction(action);
    }
    if (ChatHelper::parseValue("command", filtered).substr(0, 3) == "do ")
    {
        std::string action = ChatHelper::parseValue("command", filtered);
        action = action.substr(3);
        DoSpecificAction(action);
    }
    else if (type != CHAT_MSG_WHISPER && filtered.size() > 6 && filtered.substr(0, 6) == "queue ")
    {
        std::string remaining = filtered.substr(filtered.find(" ") + 1);
        int index = 1;
        Group* group = bot->GetGroup();
        if (group)
        {
            for (GroupReference *ref = group->GetFirstMember(); ref; ref = ref->next())
            {
                if (ref->getSource() == master)
                    continue;

                if (ref->getSource() == bot)
                    break;

                index++;
            }
        }
        ChatCommandHolder cmd(remaining, &fromPlayer, type, time(0) + index);
        chatCommands.push(cmd);
    }
    else if (filtered == "reset")
    {
        Reset(true);
    }
    else if (filtered == "logout")
    {
        if (!(bot->IsStunnedByLogout() || bot->GetSession()->isLogingOut()))
        {
            if (type == CHAT_MSG_WHISPER)
                TellPlayer(&fromPlayer, BOT_TEXT("logout_start"));

            if (master && master->GetPlayerbotMgr())
                SetShouldLogOut(true);
        }
    }
    else if (filtered == "logout cancel")
    {
        if (bot->IsStunnedByLogout() || bot->GetSession()->isLogingOut())
        {
            if (type == CHAT_MSG_WHISPER)
                TellPlayer(&fromPlayer, BOT_TEXT("logout_cancel"));

            WorldPacket p;
            bot->GetSession()->HandleLogoutCancelOpcode(p);
            SetShouldLogOut(false);
        }
    }
    else if ((filtered.size() > 5) && (filtered.substr(0, 5) == "wait ") && (filtered.find("wait for attack") == std::string::npos))
    {
        std::string remaining = filtered.substr(filtered.find(" ") + 1);
        uint32 delay = atof(remaining.c_str()) * IN_MILLISECONDS;
        if (delay > 20000)
        {
            TellPlayer(&fromPlayer, "Max wait time is 20 seconds!");
            return;
        }

        IncreaseAIInternalUpdateDelay(delay);
        isWaiting = true;
        TellPlayer(&fromPlayer, "Waiting for " + remaining + " seconds!");
        return;
    }
    else
    {
        ChatCommandHolder cmd(filtered, &fromPlayer, type);
        chatCommands.push(cmd);
    }
}

void PlayerbotAI::HandleBotOutgoingPacket(const WorldPacket& packet)
{
    //if (packet.empty())
    //    return;

	switch (packet.GetOpcode())
	{
	case SMSG_SPELL_FAILURE:
	{
		WorldPacket p(packet);
		p.rpos(0);
		ObjectGuid casterGuid;
        p >> casterGuid.ReadAsPacked();
		if (casterGuid != bot->GetObjectGuid())
			return;

		uint32 spellId;
		p >> spellId;
		SpellInterrupted(spellId);
		return;
	}
	case SMSG_SPELL_DELAYED:
	{
		WorldPacket p(packet);
		p.rpos(0);
		ObjectGuid casterGuid;
        p >> casterGuid.ReadAsPacked();

		if (casterGuid != bot->GetObjectGuid())
			return;

		uint32 delaytime;
		p >> delaytime;
		if (delaytime <= 1000)
            IncreaseAIInternalUpdateDelay(delaytime);
		return;
	}
    case SMSG_EMOTE: // do not react to NPC emotes
    {
        WorldPacket p(packet);
        ObjectGuid source;
        uint32 emoteId;
        p.rpos(0);
        p >> emoteId >> source;
        if (source.IsPlayer())
            botOutgoingPacketHandlers.AddPacket(packet);

        return;
    }
    case SMSG_MESSAGECHAT: // do not react to self or if not ready to reply
    {
        if (!AllowActivity())
            return;

        WorldPacket p(packet);
#ifndef MANGOSBOT_ZERO
        if (!p.empty() && (p.GetOpcode() == SMSG_MESSAGECHAT || p.GetOpcode() == SMSG_GM_MESSAGECHAT))
#else
        if (!p.empty() && p.GetOpcode() == SMSG_MESSAGECHAT)
#endif
        {
            p.rpos(0);
            uint8 msgtype, chatTag;
            uint32 lang, textLen, unused;
            ObjectGuid guid1, guid2;
            std::string name, chanName, message;
            p >> msgtype >> lang;

            // filter msg type
            switch (msgtype)
            {
            case CHAT_MSG_CHANNEL:
            case CHAT_MSG_SAY:
            case CHAT_MSG_PARTY:
            case CHAT_MSG_YELL:
            case CHAT_MSG_WHISPER:
            case CHAT_MSG_GUILD:
                break;
            default:
                return;
                break;
            }

#ifdef MANGOSBOT_ZERO
            switch (msgtype)
            {
            case CHAT_MSG_SAY:
            case CHAT_MSG_PARTY:
            case CHAT_MSG_YELL:
                p >> guid1 >> guid2;
                break;
            case CHAT_MSG_CHANNEL:
                p >> chanName >> unused >> guid1;
                break;
            default:
                p >> guid1;
                break;
            }

            if (guid1.IsEmpty() || p.size() > 0x1000)
                return;

            p >> textLen >> message >> chatTag;
#endif
#ifdef MANGOSBOT_ONE
            p >> guid1 >> unused;
            if (guid1.IsEmpty() || p.size() > 0x1000)
                return;

            switch (msgtype)
            {
            case CHAT_MSG_CHANNEL:
                p >> chanName;
                [[fallthrough]];
            case CHAT_MSG_SAY:
            case CHAT_MSG_PARTY:
            case CHAT_MSG_YELL:
            case CHAT_MSG_WHISPER:
            case CHAT_MSG_GUILD:
                p >> guid2;
                p >> textLen >> message >> chatTag;
                break;
            default:
                break;
            }
#endif
#ifdef MANGOSBOT_TWO
            p >> guid1 >> unused;
            if (guid1.IsEmpty() || p.size() > 0x1000)
                return;

            if (p.GetOpcode() == SMSG_GM_MESSAGECHAT)
            {
                p >> textLen;
                p >> name;
            }

            switch (msgtype)
            {
            case CHAT_MSG_CHANNEL:
                p >> chanName;
                [[fallthrough]];
            case CHAT_MSG_SAY:
            case CHAT_MSG_PARTY:
            case CHAT_MSG_YELL:
            case CHAT_MSG_WHISPER:
            case CHAT_MSG_GUILD:
                p >> guid2;
                p >> textLen >> message >> chatTag;
                break;
            default:
                break;
            }
#endif

            if (guid1 != bot->GetObjectGuid()) // do not reply to self
            {
                // try to always reply to real player
                time_t lastChat = GetAiObjectContext()->GetValue<time_t>("last said", "chat")->Get();
                bool isPaused = time(0) < lastChat;
                bool shouldReply = false;
                bool isFreeBot = false;
                sObjectMgr.GetPlayerNameByGUID(guid1, name);
                uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guid1);
                isFreeBot = sPlayerbotAIConfig.IsInRandomAccountList(accountId);
                if (!isFreeBot)
                    isFreeBot = sPlayerbotAIConfig.IsFreeAltBot(guid1);

                bool isMentioned = message.find(bot->GetName()) != std::string::npos;

                // random bot speaks, chat CD
                if (isFreeBot && isPaused)
                    return;
                // BG: react only if mentioned or if not channel and real player spoke
                if (bot->InBattleGround() && !(isMentioned || (msgtype != CHAT_MSG_CHANNEL && !isFreeBot)))
                    return;

                if (HasRealPlayerMaster() && guid1 != GetMaster()->GetObjectGuid())
                    return;

                if (isFreeBot && urand(0, 20))
                    return;

                if (lang == LANG_ADDON)
                    return;

                if (msgtype == CHAT_MSG_GUILD && (!sPlayerbotAIConfig.guildRepliesRate || frand(0, 100) > sPlayerbotAIConfig.guildRepliesRate))
                    return;

                if (!isFreeBot)
                {
                    if (!isMentioned && urand(0, 4))
                        return;
                }
                else
                {
                    if (isPaused)
                        return;

                    if (urand(0, 20 + 10 * isMentioned))
                        return;
                }

                QueueChatResponse(msgtype, guid1, ObjectGuid(), message, chanName, name);
                GetAiObjectContext()->GetValue<time_t>("last said", "chat")->Set(time(0) + urand(5, 25));
                return;
            }
        }

        return;
    }
    case SMSG_MOVE_KNOCK_BACK: // handle knockbacks
    {
        WorldPacket p(packet);
        p.rpos(0);

        ObjectGuid guid;
        uint32 counter;
        float vcos, vsin, horizontalSpeed, verticalSpeed = 0.f;

        p >> guid.ReadAsPacked() >> counter >> vcos >> vsin >> horizontalSpeed >> verticalSpeed;
        verticalSpeed = -verticalSpeed;

        // calculate rough knockback time
        float moveTimeHalf = verticalSpeed / 19.29f;

        float dis = 2 * moveTimeHalf * horizontalSpeed;
        float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -verticalSpeed);
        float disHalf = dis / 3.0f;
        float ox, oy, oz;
        bot->GetPosition(ox, oy, oz);
        float fx = ox + dis * vcos;
        float fy = oy + dis * vsin;
        float fz = oz + 0.5f;
#ifdef MANGOSBOT_TWO
        bot->GetMap()->GetHitPosition(ox, oy, oz + max_height, fx, fy, fz, bot->GetPhaseMask(), -0.5f);
#else
        bot->GetMap()->GetHitPosition(ox, oy, oz + max_height, fx, fy, fz, -0.5f);
#endif
        bot->UpdateFallInformationIfNeed(bot->m_movementInfo, CMSG_MOVE_KNOCK_BACK_ACK);
        bot->UpdateAllowedPositionZ(fx, fy, fz);

        // stop casting
        InterruptSpell();

        // stop movement
        StopMoving();

        // set delay based on actual distance
        float newdis = sqrt(bot->GetDistance2d(fx, fy, DIST_CALC_NONE));
        SetAIInternalUpdateDelay((uint32)((newdis / dis) * moveTimeHalf * 4 * IN_MILLISECONDS));

        // add moveflags
#ifdef MANGOSBOT_TWO
        bot->m_movementInfo.AddMovementFlag(MOVEFLAG_PENDINGSTOP);
        bot->m_movementInfo.SetMovementFlags(MOVEFLAG_FALLING);
        bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FORWARD);
#else
        bot->m_movementInfo.SetMovementFlags(MOVEFLAG_JUMPING);
        bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FORWARD);
#endif

        // copy MovementInfo
        MovementInfo movementInfo = bot->m_movementInfo;

        // send ack
        WorldPacket ack(CMSG_MOVE_KNOCK_BACK_ACK);
        movementInfo.jump.cosAngle = vcos;
        movementInfo.jump.sinAngle = vsin;
        movementInfo.jump.zspeed = -verticalSpeed;
        movementInfo.jump.xyspeed = horizontalSpeed;
#ifdef MANGOSBOT_TWO
        ack << bot->GetObjectGuid().WriteAsPacked();
#else
        ack << bot->GetObjectGuid();
#endif
        ack << uint32(0);
        ack << movementInfo;
        bot->GetSession()->HandleMoveKnockBackAck(ack);

        // set jump destination for MSG_LAND packet
        SetJumpDestination(Position(fx, fy, fz, bot->GetOrientation()));

        //bot->SendHeartBeat();

        return;
    }
	default:
		botOutgoingPacketHandlers.AddPacket(packet);
	}
}

void PlayerbotAI::SpellInterrupted(uint32 spellid)
{
    LastSpellCast& lastSpell = aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get();
    if (!spellid || lastSpell.id != spellid)
        return;

    time_t now = time(0);
    if (now <= lastSpell.time)
        return;

    uint32 castTimeSpent = 1000 * (now - lastSpell.time);
    uint32 globalCooldown = CalculateGlobalCooldown(lastSpell.id);
    if (castTimeSpent < globalCooldown)
        SetAIInternalUpdateDelay(globalCooldown - castTimeSpent);
    else
        SetAIInternalUpdateDelay(sPlayerbotAIConfig.reactDelay);

    lastSpell.id = 0;
}

int32 PlayerbotAI::CalculateGlobalCooldown(uint32 spellid)
{
    if (!spellid)
        return 0;

    int32 globalCooldown = 0;
    SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellid);
    if (spellEntry)
    {
        globalCooldown = spellEntry->StartRecoveryTime;
    }

    return globalCooldown > 0 ? globalCooldown : sPlayerbotAIConfig.reactDelay;
}

void PlayerbotAI::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    masterIncomingPacketHandlers.AddPacket(packet);
}

void PlayerbotAI::HandleMasterOutgoingPacket(const WorldPacket& packet)
{
    masterOutgoingPacketHandlers.AddPacket(packet);
}

void PlayerbotAI::ChangeEngine(BotState type)
{
    Engine* engine = engines[(uint8)type];

    if (currentEngine != engine)
    {
        currentEngine = engine;
        currentState = type;
        ReInitCurrentEngine();

        switch (type)
        {
        case BotState::BOT_STATE_COMBAT:
            sLog.outDebug( "=== %s COMBAT ===", bot->GetName());
            break;
        case BotState::BOT_STATE_NON_COMBAT:
            sLog.outDebug( "=== %s NON-COMBAT ===", bot->GetName());
            break;
        case BotState::BOT_STATE_DEAD:
            sLog.outDebug( "=== %s DEAD ===", bot->GetName());
            break;
        case BotState::BOT_STATE_REACTION:
            sLog.outDebug("=== %s REACTION ===", bot->GetName());
            break;
        default: break;
        }
    }
}

void PlayerbotAI::DoNextAction(bool min)
{
    if (!bot->IsInWorld() || bot->IsBeingTeleported() || (GetMaster() && GetMaster()->IsBeingTeleported()))
    {
        SetAIInternalUpdateDelay(sPlayerbotAIConfig.globalCoolDown);
        return;
    }

    // if in combat but stuck with old data - clear targets
    if (currentEngine == engines[(uint8)BotState::BOT_STATE_NON_COMBAT] && sServerFacade.IsInCombat(bot))
    {
        if (aiObjectContext->GetValue<Unit*>("current target")->Get() != NULL ||
            aiObjectContext->GetValue<ObjectGuid>("attack target")->Get() != ObjectGuid() || 
            aiObjectContext->GetValue<Unit*>("dps target")->Get() != NULL)
        {
            Reset();
        }
    }

    bool minimal = !AllowActivity();

    currentEngine->DoNextAction(NULL, 0, (minimal || min), bot->IsTaxiFlying());

    if (minimal)
    {
        if(!bot->isAFK() && !bot->InBattleGround() && !HasRealPlayerMaster())
            bot->ToggleAFK();

        SetAIInternalUpdateDelay(sPlayerbotAIConfig.passiveDelay);
        return;
    }
    else if (bot->isAFK())
        bot->ToggleAFK();


    Group *group = bot->GetGroup();
    // test BG master set
    if ((!master || !HasActivePlayerMaster()) && group && !IsRealPlayer())
    {
        PlayerbotAI* ai = bot->GetPlayerbotAI();

        //Ideally we want to have the leader as master.
        Player* newMaster = ai->GetGroupMaster();
        Player* playerMaster = nullptr;

        //Are there any non-bot players in the group?
        if (!newMaster || newMaster->GetPlayerbotAI())
            for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
            {
                Player* member = gref->getSource();

                if (!member)
                    continue;

                if (member == bot)
                    continue;

                if (member == newMaster)
                    continue;

                if (!member->IsInWorld())
                    continue;

                if (!member->IsInGroup(bot, true))
                    continue;

                //Do not make bots your master if they are nog group leader.
                if (member->GetPlayerbotAI() && !bot->InBattleGround())
                    continue;

                if (bot->InBattleGround())
                    continue;

                // same BG
                if (bot->InBattleGround() && bot->GetBattleGround()->GetTypeId() == BATTLEGROUND_AV && !member->GetPlayerbotAI() && member->InBattleGround() && bot->GetMapId() == member->GetMapId())
                {
                    // TODO disable move to objective if have master in bg
                    continue;

                    if (!group->SameSubGroup(bot, member))
                        continue;

                    if (member->GetLevel() < bot->GetLevel())
                        continue;

                    // follow real player only if he has more honor/arena points
                    bool isArena = false;
#ifndef MANGOSBOT_ZERO
                    if (bot->GetBattleGround()->IsArena())
                        isArena = true;
#endif
                    if (isArena)
                    {
                        if (group->IsLeader(member->GetObjectGuid()))
                        {
                            playerMaster = member;
                            break;
                        }
                        else
                            continue;
                    }
                    else
                    {
#ifndef MANGOSBOT_ZERO
                        uint32 honorpts = member->GetHonorPoints();
                        if (bot->GetHonorPoints() && honorpts < bot->GetHonorPoints())
                            continue;
#else
                        if (bot->GetHonorRankInfo().rank >= member->GetHonorRankInfo().rank)
                            continue;
#endif
                    }

                    playerMaster = member;
                    continue;
                }

                newMaster = member;
                break;
            }

        if (!newMaster && playerMaster)
            newMaster = playerMaster;

        if (newMaster && (!master || master != newMaster) && bot != newMaster)
        {
            master = newMaster;
            ai->SetMaster(newMaster);
            ai->ResetStrategies();
            ai->ChangeStrategy("+follow", BotState::BOT_STATE_NON_COMBAT);

            if (ai->GetMaster() == ai->GetGroupMaster())
                ai->TellPlayer(master, BOT_TEXT("hello_follow"));
            else
                ai->TellPlayer(master, BOT_TEXT("hello"));
        }
    }

    // fix bots in BG not having proper strategies
#ifdef MANGOSBOT_ZERO
    if (bot->InBattleGround() && !HasStrategy("battleground", BotState::BOT_STATE_NON_COMBAT))
        ResetStrategies();
#else
    if ((bot->InBattleGround() && !bot->InArena() && !HasStrategy("battleground", BotState::BOT_STATE_NON_COMBAT)) || (bot->InArena() && !HasStrategy("arena", BotState::BOT_STATE_NON_COMBAT)))
        ResetStrategies();
#endif

    if (master && master->IsInWorld())
	{       
		if (master->m_movementInfo.HasMovementFlag(MOVEFLAG_WALK_MODE) && sServerFacade.GetDistance2d(bot, master) < 20.0f) bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);
		else bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_WALK_MODE);

        if (master->IsSitState() && aiInternalUpdateDelay < 1000)
        {
            if (!sServerFacade.isMoving(bot) && sServerFacade.GetDistance2d(bot, master) < 10.0f)
                bot->SetStandState(UNIT_STAND_STATE_SIT);
        }
        else if (aiInternalUpdateDelay < 1000)
            bot->SetStandState(UNIT_STAND_STATE_STAND);

        if (!group && sRandomPlayerbotMgr.IsFreeBot(bot))
        {
            bot->GetPlayerbotAI()->SetMaster(nullptr);
        }
	}
	else if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_WALK_MODE)) bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_WALK_MODE);
    else if ((aiInternalUpdateDelay < 1000) && bot->IsSitState()) bot->SetStandState(UNIT_STAND_STATE_STAND);

#ifndef MANGOSBOT_ZERO
    if (bot->IsFlying() && !bot->IsFreeFlying())
    {
        if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING))
            bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
#ifdef MANGOSBOT_ONE
        if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING2))
            bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING2);
#endif
        if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_CAN_FLY))
            bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
        if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_LEVITATING))
            bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_LEVITATING);
    }
#endif

    // land after knockback/jump
#ifdef MANGOSBOT_TWO
    if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLING))
#else
    if (bot->m_movementInfo.HasMovementFlag(MOVEFLAG_JUMPING))
#endif
    {
        //// stop movement
        //bot->InterruptMoving(true);
        //bot->GetMotionMaster()->Clear();
        //bot->GetMotionMaster()->MoveIdle();

        // remove moveflags
#ifdef MANGOSBOT_TWO
        bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FALLING);
        bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_PENDINGSTOP);
#else
        bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_JUMPING);
#endif

        // set jump destination
        bot->m_movementInfo.pos = !GetJumpDestination().IsEmpty() ? GetJumpDestination() : bot->GetPosition();
        bot->m_movementInfo.jump = MovementInfo::JumpInfo();

        WorldPacket land(MSG_MOVE_FALL_LAND);
#ifdef MANGOSBOT_TWO
        land << bot->GetObjectGuid().WriteAsPacked();
#endif
        land << bot->m_movementInfo;
        bot->GetSession()->HandleMovementOpcodes(land);

        // move stop
        WorldPacket stop(MSG_MOVE_STOP);
#ifdef MANGOSBOT_TWO
        stop << bot->GetObjectGuid().WriteAsPacked();
#endif
        stop << bot->m_movementInfo;
        bot->GetSession()->HandleMovementOpcodes(stop);

        // stop movement
        StopMoving();

        ResetJumpDestination();
    }

    if (bot->IsTaxiFlying())
    {        
        return;
    }

    // random jumping (WIP, not working properly)
    bool randomJump = false;
    if (randomJump && !inCombat /*&& (!master || (master && sServerFacade.GetDistance2d(bot, master) < 20.0f)) && !bot->m_movementInfo.HasMovementFlag(MOVEFLAG_JUMPING) && !bot->IsMoving()*/)
    {
        if (!urand(0, 50))
        {
            //float dx, dy, dz = 0.f;
            //if (bot->IsMoving())
            //    bot->GetMotionMaster()->GetDestination(dx, dy, dz);

            float angle = master ? master->GetOrientation() : bot->GetOrientation();
            //if (angle > M_PI_F)
            //    angle -= 2.0f * M_PI_F;

            float vsin = 1.f;// sin(angle);
            float vcos = 0.f;// cos(angle);

            // calculate jump time
            float moveTimeHalf = 7.96f / 19.29f;

            // calculate jump distance
            float dis = 0.f;// 2 * moveTimeHalf * bot->GetSpeed(MOVE_RUN);
            float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -7.96f);

            // calculate jump destination
            float ox, oy, oz;
            bot->GetPosition(ox, oy, oz);
            float fx = ox + dis * vsin;
            float fy = oy + dis * vcos;
            float fz = oz +0.5f;
#ifdef MANGOSBOT_TWO
            bot->GetMap()->GetHitPosition(ox, oy, oz + 2.5f, fx, fy, fz, bot->GetPhaseMask(), -0.5f);
#else
            bot->GetMap()->GetHitPosition(ox, oy, oz + max_height, fx, fy, fz, -0.5f);
#endif
            bot->UpdateFallInformationIfNeed(bot->m_movementInfo, MSG_MOVE_JUMP);
            bot->UpdateAllowedPositionZ(fx, fy, fz);
            // set jump destination for MSG_LAND packet
            SetJumpDestination(Position(fx, fy, fz, bot->GetOrientation()));

            // set delay based on actual distance
            //float newdis = sqrt(bot->GetDistance2d(fx, fy, DIST_CALC_NONE));
            //SetAIInternalUpdateDelay((uint32)((newdis / dis)* moveTimeHalf * 4 * IN_MILLISECONDS));

            // stop movement
            StopMoving();

            // set delay based on actual distance
            float newdis = sqrt(bot->GetDistance2d(fx, fy, DIST_CALC_NONE));
            SetAIInternalUpdateDelay((uint32)((newdis / dis)* moveTimeHalf * 4 * IN_MILLISECONDS));

            // jump packet
            WorldPacket jump(MSG_MOVE_JUMP);

            //// add moveflags
            //bot->m_movementInfo.SetMovementFlags(MOVEFLAG_JUMPING);
            //bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FORWARD);

            // copy MovementInfo
            MovementInfo movementInfo = bot->m_movementInfo;

            // write jump info
            movementInfo.jump.zspeed = -7.96f;
            movementInfo.jump.cosAngle = vcos;
            movementInfo.jump.sinAngle = vsin;
            movementInfo.jump.xyspeed = 0.f;// bot->GetSpeed(MOVE_RUN);
            //movementInfo.jump.start = movementInfo.pos;
            //movementInfo.jump.startClientTime = WorldTimer::getMSTime();
            //movementInfo.pos = bot->GetPosition();

            // write packet info
#ifndef MANGOSBOT_ZERO
            jump << bot->GetObjectGuid().WriteAsPacked();
#endif
            jump << movementInfo;
            bot->GetSession()->HandleMovementOpcodes(jump);
            //bot->SendHeartBeat();

            //bot->m_movementInfo.ChangePosition(fx, fy, fz, bot->GetOrientation());

            // add moveflag
            //bot->m_movementInfo.AddMovementFlag(MOVEFLAG_PENDINGSTOP);
            //bot->SendHeartBeat();
        }
    }
}

void PlayerbotAI::ReInitCurrentEngine()
{
    InterruptSpell();
    currentEngine->Init();
}

void PlayerbotAI::ChangeStrategy(const string& names, BotState type)
{
    if(type == BotState::BOT_STATE_ALL)
    {
        for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
        {
            Engine* engine = engines[i];
            if (engine)
            {
                engine->ChangeStrategy(names);
            }
        }
    }
    else
    {
        Engine* engine = engines[(uint8)type];
        if (engine)
        {
            engine->ChangeStrategy(names);
        }
    }
}

void PlayerbotAI::PrintStrategies(Player* requester, BotState type)
{
    if (type == BotState::BOT_STATE_ALL)
    {
        for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
        {
            Engine* engine = engines[i];
            if (engine)
            {
                engine->PrintStrategies(requester, BotStateToString(BotState(i)));
            }
        }
    }
    else
    {
        Engine* engine = engines[(uint8)type];
        if (engine)
        {
            engine->PrintStrategies(requester, BotStateToString(type));
        }
    }
}

void PlayerbotAI::ClearStrategies(BotState type)
{
    if (type == BotState::BOT_STATE_ALL)
    {
        for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
        {
            Engine* engine = engines[i];
            if (engine)
            {
                engine->removeAllStrategies();
            }
        }
    }
    else
    {
        Engine* engine = engines[(uint8)type];
        if (engine)
        {
            engine->removeAllStrategies();
        }
    }
}

list<string_view> PlayerbotAI::GetStrategies(BotState type)
{
    // Can't get all strategies for all engines
    if (type != BotState::BOT_STATE_ALL)
    {
        Engine* engine = engines[(uint8)type];
        if (engine)
        {
            return engine->GetStrategies();
        }
    }

    return list<string_view>();
}

bool PlayerbotAI::CanDoSpecificAction(const string& name, bool isUseful, bool isPossible)
{
    for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
    {
        Engine* engine = engines[i];
        if(engine)
        {
            if(engine->CanExecuteAction(name, isUseful, isPossible))
            {
                return true;
            }
        }
    }

    return false;
}

bool PlayerbotAI::DoSpecificAction(const string& name, Event event, bool silent)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    for (uint8 i = 0 ; i < (uint8)BotState::BOT_STATE_ALL; i++)
    {
        Engine* engine = engines[i];
        if(engine)
        {
            ActionResult res = engine->ExecuteAction(name, event);
            switch (res)
            {
                case ACTION_RESULT_OK:
                {
                    if (!silent)
                    {
                        PlaySound(TEXTEMOTE_NOD);
                    }

                    return true;
                }

                case ACTION_RESULT_IMPOSSIBLE:
                {
                    if (!silent)
                    {
                        ostringstream out;
                        out << name << ": impossible";
                        TellError(requester, out.str());
                        PlaySound(TEXTEMOTE_NO);
                    }

                    return false;
                }

                case ACTION_RESULT_USELESS:
                {
                    if (!silent)
                    {
                        ostringstream out;
                        out << name << ": useless";
                        TellError(requester, out.str());
                        PlaySound(TEXTEMOTE_NO);
                    }

                    return false;
                }

                case ACTION_RESULT_FAILED:
                {
                    if (!silent)
                    {
                        ostringstream out;
                        out << name << ": failed";
                        TellError(requester, out.str());
                    }

                    return false;
                }

                default:
                {
                    if (!silent)
                    {
                        ostringstream out;
                        out << name << ": unknown action";
                        TellError(requester, out.str());
                    }

                    return false;
                }
            }
        }
        else
        {
            if (!silent)
            {
                ostringstream out;
                out << name << ": engine not ready";
                TellError(requester, out.str());
            }
        }
    }

    return false;
}

bool PlayerbotAI::PlaySound(uint32 emote)
{
    if (EmotesTextSoundEntry const* soundEntry = FindTextSoundEmoteFor(emote, bot->getRace(), bot->getGender()))
    {
        bot->PlayDistanceSound(soundEntry->SoundId);
        return true;
    }

    return false;
}

bool PlayerbotAI::PlayEmote(uint32 emote)
{
    WorldPacket data(SMSG_TEXT_EMOTE);
    data << (TextEmotes)emote;
    data << urand(0, EmoteAction::GetNumberOfEmoteVariants((TextEmotes)emote, bot->getRace(), bot->getGender()) - 1);
    data << ((master && (sServerFacade.GetDistance2d(bot, master) < 30.0f) && urand(0, 1)) ? master->GetObjectGuid() : (bot->GetSelectionGuid() && urand(0, 1)) ? bot->GetSelectionGuid() : ObjectGuid());
    bot->GetSession()->HandleTextEmoteOpcode(data);

    return false;
}

bool PlayerbotAI::ContainsStrategy(StrategyType type)
{
    for (uint8 i = 0 ; i < (uint8)BotState::BOT_STATE_ALL; i++)
    {
        if(engines[i])
        {
            if (engines[i]->ContainsStrategy(type))
            {
                return true;
            }
        }
    }

    return false;
}

bool PlayerbotAI::HasStrategy(const string& name, BotState type)
{
    // Can't check the strategy for all engines at once
    if(type != BotState::BOT_STATE_ALL)
    {
        const uint8 typeIndex = (uint8)type;
        if (engines[typeIndex])
        {
            return engines[typeIndex]->HasStrategy(name);
        }
    }

    return false;
}

void PlayerbotAI::ResetStrategies(bool autoLoad)
{
    for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
    {
        engines[i]->initMode = true;
        engines[i]->removeAllStrategies();
    }

    AiFactory::AddDefaultCombatStrategies(bot, this, engines[(uint8)BotState::BOT_STATE_COMBAT]);
    AiFactory::AddDefaultNonCombatStrategies(bot, this, engines[(uint8)BotState::BOT_STATE_NON_COMBAT]);
    AiFactory::AddDefaultDeadStrategies(bot, this, engines[(uint8)BotState::BOT_STATE_DEAD]);
    AiFactory::AddDefaultReactionStrategies(bot, this, reactionEngine);
    if (autoLoad && HasPlayerRelation()) sPlayerbotDbStore.Load(this);

    for (uint8 i = 0; i < (uint8)BotState::BOT_STATE_ALL; i++)
    {
        engines[i]->initMode = false;
        engines[i]->Init();
    }
}

bool PlayerbotAI::IsRanged(Player* player, bool inGroup)
{
    PlayerbotAI* botAi = player->GetPlayerbotAI();
    if (botAi)
    {
        bool isRanged = botAi->ContainsStrategy(STRATEGY_TYPE_RANGED);
        if (inGroup || isRanged)
            return isRanged;
    }

    switch (player->getClass())
    {
    case CLASS_PALADIN:
    case CLASS_WARRIOR:
    case CLASS_ROGUE:
#ifdef MANGOSBOT_TWO
    case CLASS_DEATH_KNIGHT:
#endif
        return false;
    case CLASS_DRUID:
        return !HasAnyAuraOf(player, "cat form", "bear form", "dire bear form", NULL);
    }
    return true;
}

bool PlayerbotAI::IsTank(Player* player, bool inGroup)
{
    PlayerbotAI* botAi = player->GetPlayerbotAI();
    if (botAi)
    {
        bool isTank = botAi->ContainsStrategy(STRATEGY_TYPE_TANK);
        if (inGroup || isTank)
            return isTank;
    }
       
    BotRoles botRoles = AiFactory::GetPlayerRoles(player);

    return (botRoles & BOT_ROLE_TANK) != 0;
}

bool PlayerbotAI::IsHeal(Player* player, bool inGroup)
{
    PlayerbotAI* botAi = player->GetPlayerbotAI();
    if (botAi)
    {
        bool isHeal = botAi->ContainsStrategy(STRATEGY_TYPE_HEAL);
        if (inGroup || isHeal)
            return isHeal;
    }

    BotRoles botRoles = AiFactory::GetPlayerRoles(player);

    return (botRoles & BOT_ROLE_HEALER) != 0;
}

namespace MaNGOS
{

    class UnitByGuidInRangeCheck
    {
    public:
        UnitByGuidInRangeCheck(WorldObject const* obj, ObjectGuid guid, float range) : i_obj(obj), i_range(range), i_guid(guid) {}
        WorldObject const& GetFocusObject() const { return *i_obj; }
        bool operator()(Unit* u)
        {
            return u->GetObjectGuid() == i_guid && i_obj->IsWithinDistInMap(u, i_range);
        }
    private:
        WorldObject const* i_obj;
        float i_range;
        ObjectGuid i_guid;
    };

    class GameObjectByGuidInRangeCheck
    {
    public:
        GameObjectByGuidInRangeCheck(WorldObject const* obj, ObjectGuid guid, float range) : i_obj(obj), i_range(range), i_guid(guid) {}
        WorldObject const& GetFocusObject() const { return *i_obj; }
        bool operator()(GameObject* u)
        {
            if (u && i_obj->IsWithinDistInMap(u, i_range) && sServerFacade.isSpawned(u) && u->GetGOInfo() && u->GetObjectGuid() == i_guid)
                return true;

            return false;
        }
    private:
        WorldObject const* i_obj;
        float i_range;
        ObjectGuid i_guid;
    };

};


Unit* PlayerbotAI::GetUnit(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    Map* map = bot->GetMap();
    if (!map)
        return NULL;

    return sObjectAccessor.GetUnit(*bot, guid);
}

Unit* PlayerbotAI::GetUnit(CreatureDataPair const* creatureDataPair)
{
    if (!creatureDataPair)
        return NULL;

    ObjectGuid guid(HIGHGUID_UNIT, creatureDataPair->second.id, creatureDataPair->first);

    if (!guid)
        return NULL;

    Map* map = sMapMgr.FindMap(creatureDataPair->second.mapid);

    if (!map)
        return NULL;

    return map->GetUnit(guid);
}


Creature* PlayerbotAI::GetCreature(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    Map* map = bot->GetMap();
    if (!map)
        return NULL;

    return map->GetCreature(guid);
}

GameObject* PlayerbotAI::GetGameObject(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    Map* map = bot->GetMap();
    if (!map)
        return NULL;

    return map->GetGameObject(guid);
}

GameObject* PlayerbotAI::GetGameObject(GameObjectDataPair const* gameObjectDataPair)
{
    if (!gameObjectDataPair)
        return NULL;

    ObjectGuid guid(HIGHGUID_GAMEOBJECT, gameObjectDataPair->second.id, gameObjectDataPair->first);

    if (!guid)
        return NULL;

    Map* map = sMapMgr.FindMap(gameObjectDataPair->second.mapid);

    if (!map)
        return NULL;

    return map->GetGameObject(guid);
}

WorldObject* PlayerbotAI::GetWorldObject(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    Map* map = bot->GetMap();
    if (!map)
        return NULL;

    return map->GetWorldObject(guid);
}

vector<Player*> PlayerbotAI::GetPlayersInGroup()
{
    vector<Player*> members;

    Group* group = bot->GetGroup();

    if (!group)
        return members;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->getSource();

        if (member->GetPlayerbotAI() && !member->GetPlayerbotAI()->IsRealPlayer())
            continue;

        members.push_back(ref->getSource());
    }   

    return members;
}

bool PlayerbotAI::TellPlayerNoFacing(Player* player, string text, PlayerbotSecurityLevel securityLevel, bool isPrivate, bool noRepeat)
{
    if(!player)
        return false;

    time_t lastSaid = whispers[text];
    if (!noRepeat || !lastSaid || (time(0) - lastSaid) >= sPlayerbotAIConfig.repeatDelay / 1000)
    {
        whispers[text] = time(0);

        vector<Player*> recievers;

        ChatMsg type = CHAT_MSG_SYSTEM;

        if (!isPrivate && bot->GetGroup())
        {
            recievers = GetPlayersInGroup();

            if(!recievers.empty())
                type = bot->GetGroup()->IsRaidGroup() ? CHAT_MSG_RAID : CHAT_MSG_PARTY;
        }

        if (type == CHAT_MSG_SYSTEM && HasRealPlayerMaster())
            type = CHAT_MSG_WHISPER;

        if (type == CHAT_MSG_SYSTEM && (sPlayerbotAIConfig.randomBotSayWithoutMaster || HasStrategy("debug", BotState::BOT_STATE_NON_COMBAT)))
            type = CHAT_MSG_SAY;

        WorldPacket data;

        switch (type) {
        case CHAT_MSG_SAY:
            bot->Say(text, (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
            return true;
        case CHAT_MSG_RAID:
        case CHAT_MSG_PARTY:
            ChatHandler::BuildChatPacket(data, type, text.c_str(), LANG_UNIVERSAL, CHAT_TAG_NONE, bot->GetObjectGuid(), bot->GetName());

            for (auto reciever : recievers)
                sServerFacade.SendPacket(reciever, data);

            return true;
        case CHAT_MSG_WHISPER:
            if (!IsTellAllowed(player, securityLevel))
                return false;

            if (!HasRealPlayerMaster())
                return false;

            whispers[text] = time(0);

            if (currentChat.second >= time(0))
               type = currentChat.first;

            if (type == CHAT_MSG_ADDON)
                text = "BOT\t" + text;

            ChatHandler::BuildChatPacket(data, type == CHAT_MSG_ADDON ? CHAT_MSG_PARTY : type, text.c_str(), type == CHAT_MSG_ADDON ? LANG_ADDON : LANG_UNIVERSAL, CHAT_TAG_NONE, bot->GetObjectGuid(), bot->GetName());
            sServerFacade.SendPacket(player, data);
        }
    }

    return true;
}

bool PlayerbotAI::TellError(Player* player, string text, PlayerbotSecurityLevel securityLevel)
{
    if (!IsTellAllowed(player, securityLevel) || !IsSafe(player) || player->GetPlayerbotAI())
        return false;

    PlayerbotMgr* mgr = player->GetPlayerbotMgr();
    if (mgr) mgr->TellError(bot->GetName(), text);

    return false;
}

bool PlayerbotAI::IsTellAllowed(Player* player, PlayerbotSecurityLevel securityLevel)
{
    if (!player || player->IsBeingTeleported())
        return false;

    if (!GetSecurity()->CheckLevelFor(securityLevel, true, player))
        return false;

    if (sPlayerbotAIConfig.whisperDistance && !bot->GetGroup() && sRandomPlayerbotMgr.IsFreeBot(bot) &&
            player->GetSession()->GetSecurity() < SEC_GAMEMASTER &&
            (bot->GetMapId() != player->GetMapId() || sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.whisperDistance))
        return false;

    return true;
}

bool PlayerbotAI::TellPlayer(Player* player, string text, PlayerbotSecurityLevel securityLevel, bool isPrivate)
{
    if (!TellPlayerNoFacing(player, text, securityLevel, isPrivate))
        return false;

    if (player && !player->IsBeingTeleported() && !sServerFacade.isMoving(bot) && !sServerFacade.IsInCombat(bot) && bot->GetMapId() == player->GetMapId() && !bot->IsTaxiFlying() && !bot->IsFlying())
    {
        if (!sServerFacade.IsInFront(bot, player, sPlayerbotAIConfig.sightDistance, EMOTE_ANGLE_IN_FRONT))
            sServerFacade.SetFacingTo(bot, player);

        bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
    }

    return true;
}

bool IsRealAura(Player* bot, Aura const* aura, Unit* unit)
{
    if (!aura)
        return false;

    if (!sServerFacade.IsHostileTo(unit, bot))
        return true;

    uint32 stacks = aura->GetStackAmount();
    if (stacks >= aura->GetSpellProto()->StackAmount)
        return true;

    if (aura->GetCaster() == bot || IsPositiveSpell(aura->GetSpellProto()) || aura->IsAreaAura())
        return true;

    return false;
}

bool PlayerbotAI::HasAura(string name, Unit* unit, bool maxStack, bool checkIsOwner, int maxAuraAmount, bool hasMyAura, int minDuration, int auraTypeId)
{
    if (!unit)
        return false;

    wstring wnamepart;

    vector<uint32> ids = chatHelper.SpellIds(name);

    if (ids.empty())
    {
        //sLog.outError("Please add %s to spell list", name.c_str());
        if (!Utf8toWStr(name, wnamepart))
            return 0;

        wstrToLower(wnamepart);
    }

    int auraAmount = 0;

	for (uint32 auraType = SPELL_AURA_BIND_SIGHT; auraType < TOTAL_AURAS; auraType++)
	{
        if (auraTypeId != TOTAL_AURAS && auraType != auraTypeId)
            continue;

		Unit::AuraList const& auras = unit->GetAurasByType((AuraType)auraType);

        if (auras.empty())
            continue;

        for (Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); i++)
        {
            Aura* aura = *i;
            if (!aura)
                continue;

            if (ids.empty())
            {
                const string auraName = aura->GetSpellProto()->SpellName[0];
                if (auraName.empty() || auraName.length() != wnamepart.length() || !Utf8FitTo(auraName, wnamepart))
                    continue;
            }
            else
            {
                if (std::find(ids.begin(), ids.end(), aura->GetSpellProto()->Id) == ids.end())
                    continue;
            }

			if (IsRealAura(bot, aura, unit))
            {
                if (hasMyAura && aura->GetHolder())
                {
                    if (aura->GetHolder()->GetCasterGuid() == bot->GetObjectGuid())
                        return true;
                    else
                        continue;
                }

                if (checkIsOwner && aura->GetHolder())
                {
                    if (aura->GetHolder()->GetCasterGuid() != bot->GetObjectGuid())
                        continue;
                }

                uint32 maxStackAmount = aura->GetSpellProto()->StackAmount;
                uint32 maxProcCharges = aura->GetSpellProto()->procCharges;

                if (maxStack)
                {
                    if (maxStackAmount && aura->GetStackAmount() >= maxStackAmount)
                        auraAmount++;

                    if (maxProcCharges && aura->GetHolder()->GetAuraCharges() >= maxProcCharges)
                        auraAmount++;
                }
                else
                    auraAmount++;

                bool minDurationPassed = true;

                if (minDuration > 0)
                {
                    int32 auraDuration = aura->GetHolder()->GetAuraDuration();
                    minDurationPassed = minDuration <= auraDuration;
                }

                if (maxAuraAmount < 0 && minDurationPassed)
                    return auraAmount > 0;
            }
		}
    }

    if (maxAuraAmount >= 0)
    {
        return auraAmount == maxAuraAmount || (auraAmount > 0 && auraAmount <= maxAuraAmount);
    }

    return false;
}

bool PlayerbotAI::HasAura(uint32 spellId, Unit* unit, bool checkOwner)
{
    Aura* aura = GetAura(spellId, unit, checkOwner);
    return aura != nullptr;
}

Aura* PlayerbotAI::GetAura(uint32 spellId, Unit* unit, bool checkIsOwner)
{
    Aura* aura = nullptr;
    if (spellId != 0 && unit)
    {
        for (uint32 effect = EFFECT_INDEX_0; effect <= EFFECT_INDEX_2; effect++)
        {
            Aura* auraTmp = ((Unit*)unit)->GetAura(spellId, (SpellEffectIndex)effect);
            if (IsRealAura(bot, auraTmp, (Unit*)unit))
            {
                if (checkIsOwner)
                {
                    if (aura->GetHolder() && aura->GetHolder()->GetCasterGuid() == bot->GetObjectGuid())
                    {
                        aura = auraTmp;
                        break;
                    }
                }
                else
                {
                    aura = auraTmp;
                    break;
                }
            }
        }
    }

    return aura;
}

Aura* PlayerbotAI::GetAura(std::string name, Unit* unit, bool checkIsOwner)
{
    if (!name.empty() && unit)
    {
        wstring wnamepart;
        vector<uint32> ids = chatHelper.SpellIds(name);

        if (ids.empty())
        {
            //sLog.outError("Please add %s to spell list", name.c_str());
            if (!Utf8toWStr(name, wnamepart))
                return 0;

            wstrToLower(wnamepart);
        }

        for (uint32 auraType = SPELL_AURA_BIND_SIGHT; auraType < TOTAL_AURAS; auraType++)
        {
            Unit::AuraList const& auras = unit->GetAurasByType((AuraType)auraType);

            if (auras.empty())
                continue;

            for (Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); i++)
            {
                Aura* aura = *i;
                if (!aura)
                    continue;

                if (ids.empty())
                {
                    const string auraName = aura->GetSpellProto()->SpellName[0];
                    if (auraName.empty() || auraName.length() != wnamepart.length() || !Utf8FitTo(auraName, wnamepart))
                        continue;
                }
                else
                {
                    if (std::find(ids.begin(), ids.end(), aura->GetSpellProto()->Id) == ids.end())
                        continue;
                }

                if (IsRealAura(bot, aura, unit))
                {
                    if (checkIsOwner && aura->GetHolder())
                    {
                        if (aura->GetHolder()->GetCasterGuid() == bot->GetObjectGuid())
                        {
                            return aura;
                        }
                    }
                    else
                    {
                        return aura;
                    }
                }
            }
        }
    }

    return nullptr;
}

std::vector<Aura*> PlayerbotAI::GetAuras(Unit* unit)
{
    std::vector<Aura*> outAuras;
    for (uint32 auraType = SPELL_AURA_BIND_SIGHT; auraType < TOTAL_AURAS; auraType++)
    {
        Unit::AuraList const& auras = unit->GetAurasByType((AuraType)auraType);

        if (auras.empty())
            continue;

        for (Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); i++)
        {
            Aura* aura = *i;
            if (aura)
            {
                outAuras.push_back(aura);
            }
        }
    }

    return outAuras;
}

bool PlayerbotAI::HasAnyAuraOf(Unit* player, ...)
{
    if (!player)
        return false;

    va_list vl;
    va_start(vl, player);

    const char* cur;
    do {
        cur = va_arg(vl, const char*);
        if (cur && HasAura(cur, player)) {
            va_end(vl);
            return true;
        }
    }
    while (cur);

    va_end(vl);
    return false;
}

bool PlayerbotAI::GetSpellRange(string name, float* maxRange, float* minRange)
{
    // Copied from Spell::GetMinMaxRange
    const uint32 spellId = aiObjectContext->GetValue<uint32>("spell id", name)->Get();
    if(spellId)
    {
        const SpellEntry* spellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (spellInfo)
        {
            const SpellRangeEntry* spellRangeEntry = sServerFacade.LookupSpellRangeEntry(spellInfo->rangeIndex);
            if(spellRangeEntry)
            {
                float spellMinRange = 0.0f, spellMaxRange = 0.0f, rangeMod = 0.0f;
                if (spellRangeEntry->Flags & SPELL_RANGE_FLAG_MELEE)
                {
                    rangeMod = bot->GetCombinedCombatReach(bot, true, 0.f);
                }
                else
                {
                    float meleeRange = 0.0f;
                    if (spellRangeEntry->Flags & SPELL_RANGE_FLAG_RANGED)
                    {
                        meleeRange = bot->GetCombinedCombatReach(bot, true, 0.f);
                    }

                    spellMinRange = spellRangeEntry->minRange + meleeRange;
                    spellMaxRange = spellRangeEntry->maxRange;
                }

                if (spellInfo->HasAttribute(SPELL_ATTR_USES_RANGED_SLOT))
                {
                    if (Item* rangedWeapon = bot->GetWeaponForAttack(RANGED_ATTACK))
                    {
                        spellMaxRange *= rangedWeapon->GetProto()->RangedModRange * 0.01f;
                    }
                }

                if (Player* modOwner = bot->GetSpellModOwner())
                {
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RANGE, spellMaxRange);
                }

                spellMaxRange += rangeMod;

                if(maxRange)
                {
                    *maxRange = spellMaxRange;
                }

                if(minRange)
                {
                    *minRange = spellMinRange;
                }

                return true;
            }
        }
    }

    return false;
}

uint32 PlayerbotAI::GetSpellCastDuration(Spell* spell)
{
    uint32 spellDuration = 0;
    if(spell)
    {
        const SpellEntry* const pSpellInfo = spell->m_spellInfo;
        spellDuration = spell->GetCastTime();
        if (IsChanneledSpell(pSpellInfo))
        {
            int32 duration = GetSpellDuration(pSpellInfo);
            if (duration > 0)
            {
                spellDuration += duration;
            }
        }

        spellDuration = ceil(spellDuration);

        // fix Feign Death
        if (pSpellInfo->Id == 5384) 
        {
            spellDuration = 1000;
        }
        // fix cannibalize
        else if (pSpellInfo->Id == 20577) 
        {
            spellDuration = 10000;
        }

        uint32 globalCooldown = CalculateGlobalCooldown(pSpellInfo->Id);
        if (spellDuration < (int32)globalCooldown)
        {
            spellDuration = globalCooldown;
        }
    }

    return spellDuration + sPlayerbotAIConfig.reactDelay;
}

bool PlayerbotAI::HasSpell(string name) const
{
    return HasSpell(aiObjectContext->GetValue<uint32>("spell id", name)->Get());
}

bool PlayerbotAI::HasSpell(uint32 spellid) const
{
    Pet* pet = bot->GetPet();
    if (pet && pet->HasSpell(spellid))
    {
        return true;
    }
    else if (bot->HasSpell(spellid))
    {
        return true;
    }

    return false;
}

bool PlayerbotAI::CanCastSpell(string name, Unit* target, uint8 effectMask, Item* itemTarget, bool ignoreRange, bool ignoreInCombat)
{
    return CanCastSpell(aiObjectContext->GetValue<uint32>("spell id", name)->Get(), target, 0, true, itemTarget, ignoreRange, ignoreInCombat);
}

bool PlayerbotAI::CanCastSpell(uint32 spellid, Unit* target, uint8 effectMask, bool checkHasSpell, Item* itemTarget, bool ignoreRange, bool ignoreInCombat)
{
    if (!spellid)
        return false;

    if (bot->hasUnitState(UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL))
    {
        // Spells that can be casted while out of control
        const std::list<uint32> ignoreOutOfControllSpells = { 642, 1020, 1499, 1953, 7744, 11958, 13795, 13809, 13813, 14302, 14303, 14304, 14305, 14310, 14311, 14316, 14317, 27023, 27025, 34600, 49055, 49056, 49066, 49067 };
        if (std::find(ignoreOutOfControllSpells.begin(), ignoreOutOfControllSpells.end(), spellid) == ignoreOutOfControllSpells.end())
        {
            return false;
        }
    }

    if (!target)
        target = bot;

    if (checkHasSpell && !HasSpell(spellid))
        return false;

    Pet* pet = bot->GetPet();
    if (pet && pet->HasSpell(spellid))
        return true;

    if (!bot->IsSpellReady(spellid))
        return false;

	SpellEntry const *spellInfo = sServerFacade.LookupSpellInfo(spellid);
	if (!spellInfo)
        return false;

    // already active next melee swing spell
    if (IsNextMeleeSwingSpell(spellInfo))
    {
        Spell* autorepeatSpell = bot->GetCurrentSpell(CURRENT_MELEE_SPELL);
        if (autorepeatSpell)
        {
            if (autorepeatSpell->m_spellInfo->Id == spellInfo->Id)
                return false;
        }
    }

	if (!itemTarget)
	{
        // Consider neutral spells (spells that are neither positive or negative (e.g. feign death, hunter traps, ...)
        const std::list<uint32> neutralSpells = { 1499, 5384, 13795, 13809, 13813, 14302, 14303, 14304, 14305, 14310, 14311, 14316, 14317, 27023, 27025, 34600, 49055, 49056, 49066, 49067 };
        const bool neutralSpell = std::find(neutralSpells.begin(), neutralSpells.end(), spellid) != neutralSpells.end();
        if(!neutralSpell)
        {
            const bool positiveSpell = IsPositiveSpell(spellInfo);
            if (positiveSpell && sServerFacade.IsHostileTo(bot, target))
                return false;

            if (!positiveSpell && sServerFacade.IsFriendlyTo(bot, target))
                return false;
        }

        bool damage = false;
        for (int32 i = EFFECT_INDEX_0; i <= EFFECT_INDEX_2; i++)
        {
            // direct damage
            if (spellInfo->Effect[(SpellEffectIndex)i] == SPELL_EFFECT_SCHOOL_DAMAGE)
            {
                damage = true;
                break;
            }
            // periodic damage
            if (spellInfo->Effect[(SpellEffectIndex)i] == SPELL_EFFECT_APPLY_AURA && spellInfo->EffectBasePoints[i] >= 0)
            {
                if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_DAMAGE)
                {
                    damage = true;
                    break;
                }
            }
        }

        if (!damage)
        {
            for (int32 i = EFFECT_INDEX_0; i <= EFFECT_INDEX_2; i++)
            {
                bool immune = target->IsImmuneToSpellEffect(spellInfo, (SpellEffectIndex)i, false);
                if (immune)
                    return false;
            }
        }

        if (!ignoreRange && bot != target && sServerFacade.GetDistance2d(bot, target) > sPlayerbotAIConfig.sightDistance)
            return false;
	}

	ObjectGuid oldSel = bot->GetSelectionGuid();
	//bot->SetSelectionGuid(target->GetObjectGuid());
	Spell *spell = new Spell(bot, spellInfo, false);

    spell->m_targets.setUnitTarget(target);
    spell->SetCastItem(itemTarget ? itemTarget : aiObjectContext->GetValue<Item*>("item for spell", spellid)->Get());
    spell->m_targets.setItemTarget(spell->GetCastItem());

    SpellCastResult result = spell->CheckCast(true);
    delete spell;
	//if (oldSel)
	//	bot->SetSelectionGuid(oldSel);

    const bool ignoreReagents = HasCheat(BotCheatMask::item);

    switch (result)
    {
        case SPELL_FAILED_NOT_INFRONT:
        case SPELL_FAILED_NOT_STANDING:
        case SPELL_FAILED_UNIT_NOT_INFRONT:
        case SPELL_FAILED_MOVING:
        case SPELL_FAILED_TRY_AGAIN:
        case SPELL_CAST_OK:
            return true;
        case SPELL_FAILED_OUT_OF_RANGE:
        case SPELL_FAILED_LINE_OF_SIGHT:
            return ignoreRange;
        case SPELL_FAILED_REAGENTS:
        case SPELL_FAILED_TOTEMS:
            return ignoreReagents;
        case SPELL_FAILED_AFFECTING_COMBAT:
            return ignoreInCombat;
        default:
            return false;
    }
}

bool PlayerbotAI::CanCastSpell(uint32 spellid, GameObject* goTarget, uint8 effectMask, bool checkHasSpell, bool ignoreRange, bool ignoreInCombat)
{
    if (!spellid)
        return false;

    if (bot->hasUnitState(UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL))
        return false;

    Pet* pet = bot->GetPet();
    if (pet && pet->HasSpell(spellid))
        return true;

    if (checkHasSpell && !bot->HasSpell(spellid))
        return false;

#ifdef MANGOS
    if (bot->HasSpellCooldown(spellid))
        return false;
#endif
#ifdef CMANGOS
    if (!bot->IsSpellReady(spellid))
        return false;
#endif

    SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(spellid);
    if (!spellInfo)
        return false;

    bool damage = false;
    for (int32 i = EFFECT_INDEX_0; i <= EFFECT_INDEX_2; i++)
    {
        if (spellInfo->Effect[(SpellEffectIndex)i] == SPELL_EFFECT_SCHOOL_DAMAGE)
        {
            damage = true;
            break;
        }
    }
    if (sServerFacade.GetDistance2d(bot, goTarget) > sPlayerbotAIConfig.sightDistance)
        return false;

    //ObjectGuid oldSel = bot->GetSelectionGuid();
    bot->SetSelectionGuid(goTarget->GetObjectGuid());
    Spell* spell = new Spell(bot, spellInfo, false);

    spell->m_targets.setGOTarget(goTarget);
    spell->SetCastItem(aiObjectContext->GetValue<Item*>("item for spell", spellid)->Get());
    spell->m_targets.setItemTarget(spell->GetCastItem());

    SpellCastResult result = spell->CheckCast(true);
    delete spell;
    //if (oldSel)
    //    bot->SetSelectionGuid(oldSel);

    const bool ignoreReagents = HasCheat(BotCheatMask::item);

    switch (result)
    {
    case SPELL_FAILED_NOT_INFRONT:
    case SPELL_FAILED_NOT_STANDING:
    case SPELL_FAILED_UNIT_NOT_INFRONT:
    case SPELL_FAILED_MOVING:
    case SPELL_FAILED_TRY_AGAIN:
    case SPELL_CAST_OK:
        return true;
    case SPELL_FAILED_OUT_OF_RANGE:
        return ignoreRange;
    case SPELL_FAILED_REAGENTS:
        return ignoreReagents;
    case SPELL_FAILED_AFFECTING_COMBAT:
        return ignoreInCombat;
    default:
        return false;
    }
}

bool PlayerbotAI::CanCastSpell(uint32 spellid, float x, float y, float z, uint8 effectMask, bool checkHasSpell, Item* itemTarget, bool ignoreRange, bool ignoreInCombat)
{
    if (!spellid)
        return false;

    Pet* pet = bot->GetPet();
    if (pet && pet->HasSpell(spellid))
        return true;

    if (checkHasSpell && !bot->HasSpell(spellid))
        return false;

#ifdef MANGOS
    if (bot->HasSpellCooldown(spellid))
        return false;
#endif

    SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(spellid);
    if (!spellInfo)
        return false;

    if (!itemTarget)
    {
        if (sqrt(bot->GetDistance(x,y,z)) > sPlayerbotAIConfig.sightDistance)
            return false;
    }

    Spell* spell = new Spell(bot, spellInfo, false);

    spell->m_targets.setDestination(x, y, z);
    spell->SetCastItem(itemTarget ? itemTarget : aiObjectContext->GetValue<Item*>("item for spell", spellid)->Get());
    spell->m_targets.setItemTarget(spell->GetCastItem());

    SpellCastResult result = spell->CheckCast(true);
    delete spell;

    const bool ignoreReagents = HasCheat(BotCheatMask::item);

    switch (result)
    {
    case SPELL_FAILED_NOT_INFRONT:
    case SPELL_FAILED_NOT_STANDING:
    case SPELL_FAILED_UNIT_NOT_INFRONT:
    case SPELL_FAILED_MOVING:
    case SPELL_FAILED_TRY_AGAIN:
    case SPELL_CAST_OK:
        return true;
    case SPELL_FAILED_OUT_OF_RANGE:
        return ignoreRange;
    case SPELL_FAILED_REAGENTS:
        return ignoreReagents;
    case SPELL_FAILED_AFFECTING_COMBAT:
        return ignoreInCombat;
    default:
        return false;
    }
}

uint8 PlayerbotAI::GetHealthPercent(const Unit& target) const
{
   return (static_cast<float>(target.GetHealth()) / target.GetMaxHealth()) * 100;
}

uint8 PlayerbotAI::GetHealthPercent() const
{
   return GetHealthPercent(*bot);
}

uint8 PlayerbotAI::GetManaPercent(const Unit& target) const
{
   return (static_cast<float>(target.GetPower(POWER_MANA)) / target.GetMaxPower(POWER_MANA)) * 100;
}

uint8 PlayerbotAI::GetManaPercent() const
{
   return GetManaPercent(*bot);
}

bool PlayerbotAI::CastSpell(string name, Unit* target, Item* itemTarget, bool waitForSpell, uint32* outSpellDuration, bool canUseReagentCheat)
{
    bool result = CastSpell(aiObjectContext->GetValue<uint32>("spell id", name)->Get(), target, itemTarget, waitForSpell, outSpellDuration, canUseReagentCheat);
    if (result)
    {
        aiObjectContext->GetValue<time_t>("last spell cast time", name)->Set(time(0));
    }

    return result;
}

bool PlayerbotAI::CastSpell(uint32 spellId, Unit* target, Item* itemTarget, bool waitForSpell, uint32* outSpellDuration, bool canUseReagentCheat)
{
    if (!spellId)
        return false;

    if (!target)
        target = bot;

    Pet* pet = bot->GetPet();
	SpellEntry const *pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
	if (pet && pet->HasSpell(spellId))
    {
	    bool autocast = false;
	    for(AutoSpellList::iterator i = pet->m_autospells.begin(); i != pet->m_autospells.end(); ++i)
	    {
	        if (*i == spellId)
	        {
	            autocast = true;
	            break;
	        }
	    }

		pet->ToggleAutocast(spellId, !autocast);
		ostringstream out;
		out << (autocast ? "|cffff0000|Disabling" : "|cFF00ff00|Enabling") << " pet auto-cast for ";
		out << chatHelper.formatSpell(pSpellInfo);
        TellPlayer(GetMaster(), out);
        return true;
    }

    aiObjectContext->GetValue<LastMovement&>("last movement")->Get().Set(NULL);
    aiObjectContext->GetValue<time_t>("stay time")->Set(0);

    MotionMaster &mm = *bot->GetMotionMaster();

    if (bot->IsFlying() || bot->IsTaxiFlying())
        return false;

	//bot->clearUnitState(UNIT_STAT_CHASE);
	//bot->clearUnitState(UNIT_STAT_FOLLOW);

	bool failWithDelay = false;
    if (!bot->IsStandState())
    {
        bot->SetStandState(UNIT_STAND_STATE_STAND);
        failWithDelay = true;
    }

	ObjectGuid oldSel = bot->GetSelectionGuid();
	bot->SetSelectionGuid(target->GetObjectGuid());

    WorldObject* faceTo = target;
    if (!sServerFacade.IsInFront(bot, faceTo, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))
    {
        sServerFacade.SetFacingTo(bot, faceTo);
        //failWithDelay = true;
    }

    if (failWithDelay)
    {
        if(waitForSpell)
        {
            SetAIInternalUpdateDelay(sPlayerbotAIConfig.globalCoolDown);
        }

        if(outSpellDuration)
        {
            *outSpellDuration = sPlayerbotAIConfig.globalCoolDown;
        }

        return false;
    }

    Spell *spell = new Spell(bot, pSpellInfo, false);

    SpellCastTargets targets;
    if (pSpellInfo->Targets & TARGET_FLAG_ITEM)
    {
        spell->SetCastItem(itemTarget ? itemTarget : aiObjectContext->GetValue<Item*>("item for spell", spellId)->Get());
        targets.setItemTarget(spell->GetCastItem());

        if (bot->GetTradeData())
        {
            bot->GetTradeData()->SetSpell(spellId);
			delete spell;
            return true;
        }
    }
    else if (pSpellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
    {
        WorldLocation aoe = aiObjectContext->GetValue<WorldLocation>("aoe position")->Get();
        if (aoe.coord_x != 0)
            targets.setDestination(aoe.coord_x, aoe.coord_y, aoe.coord_z);
        else if (target && target->GetObjectGuid() != bot->GetObjectGuid())
            targets.setDestination(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
    }
    else if (pSpellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
    {
        targets.setDestination(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
    }
    else
    {
        targets.setUnitTarget(target);
    }

    if (spellId == 1953) // simulate blink coordinates
    {
        float angle = bot->GetOrientation();
        float distance = 20.0f;
        float fx = bot->GetPositionX() + cos(angle) * distance;
        float fy = bot->GetPositionY() + sin(angle) * distance;
        float fz = bot->GetPositionZ();

        float ox, oy, oz;
        bot->GetPosition(ox, oy, oz);
//#ifdef MANGOSBOT_TWO
//        bot->GetMap()->GetHitPosition(ox, oy, oz + max_height, fx, fy, fz, bot->GetPhaseMask(), -0.5f);
//#else
//        bot->GetMap()->GetHitPosition(ox, oy, oz + 2.0f, fx, fy, fz, -0.5f);
//#endif
        bot->UpdateAllowedPositionZ(fx, fy, fz);
        targets.setDestination(fx, fy, fz);
    }

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
        pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        LootObject loot = *aiObjectContext->GetValue<LootObject>("loot target");
        GameObject* go = GetGameObject(loot.guid);
        if (go && sServerFacade.isSpawned(go))
        {
            targets.setGOTarget(go);
            faceTo = go;
        }
        else
        {
            Unit* creature = GetUnit(loot.guid);
            if (creature)
            {
                targets.setUnitTarget(creature);
                faceTo = creature;
            }
        }
    }

    // Fail the cast if the bot is moving and the spell is a casting/channeled spell
    const bool isMoving = !bot->IsStopped();
    if (isMoving && ((GetSpellCastTime(pSpellInfo, bot, spell) > 0) || (IsChanneledSpell(pSpellInfo) && (GetSpellDuration(pSpellInfo) > 0))))
    {
        StopMoving();
    }

    if (HasCheat(BotCheatMask::item) && canUseReagentCheat)
    {
        for (uint8 i = 0; i < MAX_SPELL_REAGENTS; i++)
        {
            const int32 itemId = pSpellInfo->Reagent[i];
            if (itemId)
            {
                spell->m_ignoreCosts = true;
                spell->SetPowerCost(spell->CalculatePowerCost(spell->m_spellInfo, bot, spell, spell->GetCastItem(), false));
                break;              
            }
        }       
    }   

    SpellCastResult spellSuccess = spell->SpellStart(&targets);

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
        pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        LootObject loot = *aiObjectContext->GetValue<LootObject>("loot target");
        if (!loot.IsLootPossible(bot))
        {
            spell->cancel();
            //delete spell;
            return false;
        }
    }

    if (spellSuccess != SPELL_CAST_OK)
        return false;

    if (urand(0, 100) < sPlayerbotAIConfig.attackEmoteChance * 600 && bot->IsInCombat())
    {
        vector<uint32> sounds;
        sounds.push_back(TEXTEMOTE_OPENFIRE);
        sounds.push_back(305);
        sounds.push_back(307);
        PlaySound(sounds[urand(0, sounds.size() - 1)]);
    }

    if(waitForSpell)
    {
        WaitForSpellCast(spell);
    }

    if(outSpellDuration)
    {
        *outSpellDuration = GetSpellCastDuration(spell);
    }

    if (spell->GetCastTime() || (IsChanneledSpell(pSpellInfo) && GetSpellDuration(pSpellInfo) > 0))
        aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get().Set(spellId, target->GetObjectGuid(), time(0));

    aiObjectContext->GetValue<ai::PositionMap&>("position")->Get()["random"].Reset();

    if (oldSel)
        bot->SetSelectionGuid(oldSel);

    if (HasStrategy("debug spell", BotState::BOT_STATE_NON_COMBAT))
    {
        ostringstream out;
        out << "Casting " <<ChatHelper::formatSpell(pSpellInfo);
        TellPlayerNoFacing(GetMaster(), out);
    }

    return true;
}

bool PlayerbotAI::CastSpell(uint32 spellId, float x, float y, float z, Item* itemTarget, bool waitForSpell, uint32* outSpellDuration, bool canUseReagentCheat)
{
    if (!spellId)
        return false;

    Pet* pet = bot->GetPet();
    SpellEntry const* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
    if (pet && pet->HasSpell(spellId))
    {
        bool autocast = false;
        for (AutoSpellList::iterator i = pet->m_autospells.begin(); i != pet->m_autospells.end(); ++i)
        {
            if (*i == spellId)
            {
                autocast = true;
                break;
            }
        }

        pet->ToggleAutocast(spellId, !autocast);
        ostringstream out;
        out << (autocast ? "|cffff0000|Disabling" : "|cFF00ff00|Enabling") << " pet auto-cast for ";
        out << chatHelper.formatSpell(pSpellInfo);
        TellPlayer(GetMaster(), out);
        return true;
    }

    aiObjectContext->GetValue<LastMovement&>("last movement")->Get().Set(NULL);
    aiObjectContext->GetValue<time_t>("stay time")->Set(0);

    MotionMaster& mm = *bot->GetMotionMaster();

    if (bot->IsFlying() || bot->IsTaxiFlying())
        return false;

    bot->clearUnitState(UNIT_STAT_CHASE);
    bot->clearUnitState(UNIT_STAT_FOLLOW);

    bool failWithDelay = false;
    if (!bot->IsStandState())
    {
        bot->SetStandState(UNIT_STAND_STATE_STAND);
        failWithDelay = true;
    }

    ObjectGuid oldSel = bot->GetSelectionGuid();

    bool isMoving = false;
    if (!bot->GetMotionMaster()->empty())
        if (bot->GetMotionMaster()->top()->GetMovementGeneratorType() != IDLE_MOTION_TYPE)
            isMoving = true;

    if (!bot->IsStopped())
        isMoving = true;

    if (!sServerFacade.isMoving(bot) || isMoving) bot->SetFacingTo(bot->GetAngleAt(bot->GetPositionX(), bot->GetPositionY(), x, y));

    if (failWithDelay)
    {
        if (waitForSpell)
        {
            SetAIInternalUpdateDelay(sPlayerbotAIConfig.globalCoolDown);
        }

        if (outSpellDuration)
        {
            *outSpellDuration = sPlayerbotAIConfig.globalCoolDown;
        }

        return false;
    }

    Spell* spell = new Spell(bot, pSpellInfo, false);

    SpellCastTargets targets;
    if (pSpellInfo->Targets & TARGET_FLAG_ITEM)
    {
        spell->SetCastItem(itemTarget ? itemTarget : aiObjectContext->GetValue<Item*>("item for spell", spellId)->Get());
        targets.setItemTarget(spell->GetCastItem());

        if (bot->GetTradeData())
        {
            bot->GetTradeData()->SetSpell(spellId);
            delete spell;
            return true;
        }
    }
    else if (pSpellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
    {
        WorldLocation aoe = aiObjectContext->GetValue<WorldLocation>("aoe position")->Get();
        targets.setDestination(x, y, z);
    }
    else if (pSpellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
    {
        targets.setDestination(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
    }
    else
    {
        return false;
    }

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
        pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        return false;
    }

    // Prepare the reagents if cheats enabled
    if (HasCheat(BotCheatMask::item) && canUseReagentCheat)
    {
        std::vector<std::pair<int32, uint32>> spellReagents;
        for (uint8 i = 0; i < MAX_SPELL_REAGENTS; i++)
        {
            const int32 itemId = pSpellInfo->Reagent[i];
            if (itemId)
            {
                const uint32 itemAmount = pSpellInfo->ReagentCount[i];
                if (itemAmount > 0)
                {
                    const uint32 currentItemAmount = bot->GetItemCount(itemId);
                    if (currentItemAmount < itemAmount)
                    {
                        spellReagents.emplace_back((uint32)itemId, itemAmount - currentItemAmount);
                    }
                }
            }
        }

        if (!spellReagents.empty())
        {
            for (auto& pair : spellReagents)
            {
                // Check bag space and find places
                const uint32 itemId = pair.first;
                const uint32 amount = pair.second;
                ItemPosCountVec dest;
                uint8 msg = bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, amount);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = bot->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
                    bot->SendNewItem(item, amount, false, true, false);
                }
            }
        }
    }

#ifdef MANGOS
    spell->prepare(&targets);
#endif
#ifdef CMANGOS
    spell->SpellStart(&targets);
#endif

    if (sServerFacade.isMoving(bot) && spell->GetCastTime())
    {
        bot->StopMoving();
    }

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
        pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        LootObject loot = *aiObjectContext->GetValue<LootObject>("loot target");
        if (!loot.IsLootPossible(bot))
        {
            spell->cancel();
            //delete spell;
            return false;
        }
    }

    if (waitForSpell)
    {
        WaitForSpellCast(spell);
    }

    if (outSpellDuration)
    {
        *outSpellDuration = GetSpellCastDuration(spell);
    }

    aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get().Set(spellId, bot->GetObjectGuid(), time(0));
    aiObjectContext->GetValue<ai::PositionMap&>("position")->Get()["random"].Reset();

    if (oldSel)
        bot->SetSelectionGuid(oldSel);

    if (HasStrategy("debug spell", BotState::BOT_STATE_NON_COMBAT))
    {
        ostringstream out;
        out << "Casting " << ChatHelper::formatSpell(pSpellInfo);
        TellPlayerNoFacing(GetMaster(), out);
    }

    return true;
}

bool PlayerbotAI::CanCastVehicleSpell(uint32 spellId, Unit* target)
{
#ifdef MANGOSBOT_TWO
    if (!spellId)
        return false;

    TransportInfo* transportInfo = bot->GetTransportInfo();
    if (!transportInfo || !transportInfo->IsOnVehicle())
        return false;

    Unit* vehicle = (Unit*)transportInfo->GetTransport();

    // do not allow if no spells
    VehicleSeatEntry const* seat = transportInfo ? vehicle->GetVehicleInfo()->GetSeatEntry(transportInfo->GetTransportSeat()) : nullptr;
    if (seat && !seat->HasFlag(SEAT_FLAG_CAN_CAST))
        return false;

    bool canControl = seat ? (seat->HasFlag(SEAT_FLAG_CAN_CONTROL)) : false;

    if (!vehicle)
        return false;

    Unit* spellTarget = target;

    if (!spellTarget)
        spellTarget = vehicle;

    if (!spellTarget)
        return false;

#ifdef MANGOS
    if (vehicle->HasSpellCooldown(spellId))
        return false;
#endif
#ifdef CMANGOS
    if (!vehicle->IsSpellReady(spellId))
        return false;
#endif

    SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(spellId);
    if (!spellInfo)
        return false;

    // check BG siege position set in BG Tactics
    PositionEntry siegePos = GetAiObjectContext()->GetValue<ai::PositionMap&>("position")->Get()["bg siege"];

    // do not cast spell on self if spell is location based
    if (!(siegePos.isSet() || spellTarget != vehicle) && spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
        return false;

    uint32 CastingTime = !IsChanneledSpell(spellInfo) ? GetSpellCastTime(spellInfo, vehicle) : GetSpellDuration(spellInfo);

    if (CastingTime && vehicle->IsMoving())
        return false;

    if (vehicle != spellTarget && sServerFacade.GetDistance2d(vehicle, spellTarget) > 120.0f)
        return false;

    if (!target && siegePos.isSet())
    {
        if (sServerFacade.GetDistance2d(vehicle, siegePos.x, siegePos.y) > 120.0f)
            return false;
    }

    Spell* spell = new Spell(vehicle, spellInfo, false);

    WorldLocation dest;
    if (siegePos.isSet())
        dest = WorldLocation(bot->GetMapId(), siegePos.x, siegePos.y, siegePos.z, 0);
    else if (spellTarget != vehicle)
        dest = WorldLocation(spellTarget->GetMapId(), spellTarget->GetPosition());

    if (spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
        spell->m_targets.setDestination(dest.coord_x, dest.coord_y, dest.coord_z);
    else if (spellTarget != vehicle)
    {
        spell->m_targets.setUnitTarget(spellTarget);
    }

    SpellCastResult result = spell->CheckCast(true);
    delete spell;

    switch (result)
    {
    case SPELL_FAILED_NOT_INFRONT:
    case SPELL_FAILED_NOT_STANDING:
    case SPELL_FAILED_UNIT_NOT_INFRONT:
    case SPELL_FAILED_MOVING:
    case SPELL_FAILED_TRY_AGAIN:
    case SPELL_CAST_OK:
        return true;
    default:
        return false;
    }
#endif
    return false;
}

bool PlayerbotAI::CastVehicleSpell(uint32 spellId, Unit* target)
{
#ifdef MANGOSBOT_TWO
    if (!spellId)
        return false;

    TransportInfo* transportInfo = bot->GetTransportInfo();
    if (!transportInfo || !transportInfo->IsOnVehicle())
        return false;

    Unit* vehicle = (Unit*)transportInfo->GetTransport();

    // do not allow if no spells
    VehicleSeatEntry const* seat = transportInfo ? vehicle->GetVehicleInfo()->GetSeatEntry(transportInfo->GetTransportSeat()) : nullptr;
    if (!seat->HasFlag(SEAT_FLAG_CAN_CAST))
        return false;

    bool canControl = seat ? (seat->HasFlag(SEAT_FLAG_CAN_CONTROL)) : false;
    bool canTurn = seat ? (seat->HasFlag(SEAT_FLAG_ALLOW_TURNING)) : false;

    if (!vehicle)
        return false;

    Unit* spellTarget = target;

    if (!spellTarget)
        spellTarget = vehicle;

    if (!spellTarget)
        return false;

    SpellEntry const* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);

    // check BG siege position set in BG Tactics
    PositionEntry siegePos = GetAiObjectContext()->GetValue<ai::PositionMap&>("position")->Get()["bg siege"];
    if (!target && siegePos.isSet())
    {
        if (sServerFacade.GetDistance2d(vehicle, siegePos.x, siegePos.y) > 120.0f)
            return false;
    }

    // do not cast spell on self if spell is location based
    if (!(siegePos.isSet() || spellTarget != vehicle) && pSpellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
        return false;

    if (canControl)
    {
        //aiObjectContext->GetValue<LastMovement&>("last movement")->Get().Set(NULL);
        //aiObjectContext->GetValue<time_t>("stay time")->Set(0);
    }

    MotionMaster& mm = *vehicle->GetMotionMaster();

    //bot->clearUnitState(UNIT_STAT_CHASE);
    //bot->clearUnitState(UNIT_STAT_FOLLOW);

    //ObjectGuid oldSel = bot->GetSelectionGuid();
    //bot->SetSelectionGuid(target->GetObjectGuid());

    // turn vehicle if target is not in front
    bool failWithDelay = false;
    if (spellTarget != vehicle && (canControl || canTurn))
    {
        if (!sServerFacade.IsInFront(vehicle, spellTarget, 100.0f, CAST_ANGLE_IN_FRONT))
        {
            vehicle->SetFacingToObject(spellTarget);
            failWithDelay = true;
        }
    }

    if (siegePos.isSet() && (canControl || canTurn))
    {
        vehicle->SetFacingTo(vehicle->GetAngle(siegePos.x, siegePos.y));
    }

    if (failWithDelay)
    {
        SetAIInternalUpdateDelay(sPlayerbotAIConfig.globalCoolDown);
        return false;
    }

    Spell* spell = new Spell(vehicle, pSpellInfo, false);

    SpellCastTargets targets;
    if ((spellTarget != vehicle || siegePos.isSet()) && pSpellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
    {
        WorldLocation dest;
        if (spellTarget != vehicle)
            dest = WorldLocation(spellTarget->GetMapId(), spellTarget->GetPosition());
        else if (siegePos.isSet())
            dest = WorldLocation(bot->GetMapId(), siegePos.x + frand(-5.0f, 5.0f), siegePos.y + frand(-5.0f, 5.0f), siegePos.z, 0.0f);
        else
            return false;

        targets.setDestination(dest.coord_x, dest.coord_y, dest.coord_z);
        targets.setSpeed(30.0f);
        float distanceToDest = sqrt(vehicle->GetPosition().GetDistance(Position(dest.coord_x, dest.coord_y, dest.coord_z, 0.0f)));
        float elev = 0.01f;
        if (distanceToDest < 25.0f)
            elev = 0.04f;
        else if (distanceToDest < 55.0f)
            elev = 0.22f;
        else if (distanceToDest < 85.0f)
            elev = 0.42f;
        else if (distanceToDest < 95.0f)
            elev = 0.70f;
        else if (distanceToDest < 110.0f)
            elev = 0.88f;
        else
            elev = 1.0f;

        targets.setElevation(elev);
    }
    if (pSpellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
    {
        targets.setSource(vehicle->GetPositionX(), vehicle->GetPositionY(), vehicle->GetPositionZ());
    }

    if (target && !(pSpellInfo->Targets & TARGET_FLAG_DEST_LOCATION))
    {
        targets.setUnitTarget(spellTarget);
    }

#ifdef MANGOS
    spell->prepare(&targets);
#endif
#ifdef CMANGOS
    spell->SpellStart(&targets);
#endif

    if (canControl && !vehicle->IsStopped() && spell->GetCastTime())
    {
        vehicle->StopMoving();
        SetAIInternalUpdateDelay(sPlayerbotAIConfig.globalCoolDown);
        spell->cancel();
        //delete spell;
        return false;
    }

    WaitForSpellCast(spell);

    //aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get().Set(spellId, target->GetObjectGuid(), time(0));
    //aiObjectContext->GetValue<ai::PositionMap&>("position")->Get()["random"].Reset();

    if (HasStrategy("debug spell", BotState::BOT_STATE_NON_COMBAT))
    {
        ostringstream out;
        out << "Casting Vehicle Spell" << ChatHelper::formatSpell(pSpellInfo);
        TellPlayerNoFacing(GetMaster(), out);
    }

    return true;
#endif
    return false;
}

bool PlayerbotAI::IsInVehicle(bool canControl, bool canCast, bool canAttack, bool canTurn, bool fixed)
{
#ifdef MANGOSBOT_TWO
    TransportInfo* transportInfo = bot->GetTransportInfo();
    if (!transportInfo || !transportInfo->GetTransport() || !transportInfo->IsOnVehicle())
        return false;

    // get vehicle
    Unit* vehicle = (Unit*)transportInfo->GetTransport();
    if (!vehicle || !vehicle->IsAlive())
        return false;

    if (!vehicle->GetVehicleInfo())
        return false;

    // get seat
    VehicleSeatEntry const* seat = vehicle->GetVehicleInfo()->GetSeatEntry(transportInfo->GetTransportSeat());
    if (!seat)
        return false;

    if (!(canControl || canCast || canAttack || canTurn || fixed))
        return true;

    if (canControl)
        return seat->HasFlag(SEAT_FLAG_CAN_CONTROL) && (vehicle->GetVehicleInfo()->GetVehicleEntry()->m_flags & VEHICLE_FLAG_FIXED_POSITION) == 0;

    if (canCast)
        return seat->HasFlag(SEAT_FLAG_CAN_CAST);

    if (canAttack)
        return seat->HasFlag(SEAT_FLAG_CAN_ATTACK);

    if (canTurn)
        return seat->HasFlag(SEAT_FLAG_ALLOW_TURNING);

    if (fixed)
        return vehicle->GetVehicleInfo()->GetVehicleEntry()->m_flags & VEHICLE_FLAG_FIXED_POSITION;

#endif
    return false;
}

void PlayerbotAI::WaitForSpellCast(Spell *spell)
{
    SetAIInternalUpdateDelay(GetSpellCastDuration(spell));
}

void PlayerbotAI::InterruptSpell()
{
    for (int type = CURRENT_MELEE_SPELL; type < CURRENT_CHANNELED_SPELL; type++)
    {
        Spell* spell = bot->GetCurrentSpell((CurrentSpellTypes)type);
        if (!spell)
            continue;

        if (IsPositiveSpell(spell->m_spellInfo))
            continue;

        bot->InterruptSpell((CurrentSpellTypes)type);
        SpellInterrupted(spell->m_spellInfo->Id);
    }
}

bool PlayerbotAI::RemoveAura(const std::string& name)
{
    const Aura* aura = GetAura(name, bot);
    if (aura)
    {
        const uint32 spellId = aura->GetSpellProto()->Id;
        if (spellId > 0)
        {
            bot->RemoveAurasDueToSpell(spellId);
            return true;
        }
    }

    return false;
}

bool PlayerbotAI::IsInterruptableSpellCasting(Unit* target, string spell, uint8 effectMask)
{
	uint32 spellid = aiObjectContext->GetValue<uint32>("spell id", spell)->Get();
	if (!spellid || !target->IsNonMeleeSpellCasted(true))
		return false;

	SpellEntry const *spellInfo = sServerFacade.LookupSpellInfo(spellid);
	if (!spellInfo)
		return false;

	for (int32 i = EFFECT_INDEX_0; i <= EFFECT_INDEX_2; i++)
	{
		if ((spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_COMBAT) && spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
			return true;

		if ((spellInfo->Effect[i] == SPELL_EFFECT_INTERRUPT_CAST) &&
			!target->IsImmuneToSpellEffect(spellInfo, (SpellEffectIndex)i, true))
			return true;

        if ((spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA) && spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_SILENCE)
            return true;
	}

	return false;
}

bool PlayerbotAI::HasAuraToDispel(Unit* target, uint32 dispelType)
{
    bool isFriend = sServerFacade.IsFriendlyTo(bot, target);
	for (uint32 type = SPELL_AURA_NONE; type < TOTAL_AURAS; ++type)
	{
		Unit::AuraList const& auras = target->GetAurasByType((AuraType)type);
		for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
		{
			const Aura* aura = *itr;
			const SpellEntry* entry = aura->GetSpellProto();
			uint32 spellId = entry->Id;

			bool isPositiveSpell = IsPositiveSpell(spellId);
			if (isPositiveSpell && isFriend)
				continue;

			if (!isPositiveSpell && !isFriend)
				continue;

			if (sPlayerbotAIConfig.dispelAuraDuration && aura->GetAuraDuration() && aura->GetAuraDuration() < (int32)sPlayerbotAIConfig.dispelAuraDuration)
			    return false;

			if (canDispel(entry, dispelType))
				return true;
		}
	}
	return false;
}

#ifndef WIN32
inline int strcmpi(const char* s1, const char* s2)
{
    for (; *s1 && *s2 && (toupper(*s1) == toupper(*s2)); ++s1, ++s2);
    return *s1 - *s2;
}
#endif

bool PlayerbotAI::canDispel(const SpellEntry* entry, uint32 dispelType)
{
    if (entry->Dispel != dispelType)
        return false;

    return !entry->SpellName[0] ||
        (strcmpi((const char*)entry->SpellName[0], "demon skin") &&
        strcmpi((const char*)entry->SpellName[0], "mage armor") &&
        strcmpi((const char*)entry->SpellName[0], "frost armor") &&
        strcmpi((const char*)entry->SpellName[0], "wavering will") &&
        strcmpi((const char*)entry->SpellName[0], "chilled") &&
        strcmpi((const char*)entry->SpellName[0], "mana tap") &&
        strcmpi((const char*)entry->SpellName[0], "ice armor"));
}

bool PlayerbotAI::IsHealSpell(const SpellEntry* spell)
{
    // Holy Light/Flash of Light
    if (spell->SpellFamilyName == SPELLFAMILY_PALADIN)
    {
        if (spell->SpellIconID == 70 ||
            spell->SpellIconID == 242)
            return true;
    }

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        switch (spell->Effect[i])
        {
            case SPELL_EFFECT_HEAL:
            case SPELL_EFFECT_HEAL_MAX_HEALTH:
                return true;
            case SPELL_EFFECT_APPLY_AURA:
            case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
            case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
            case SPELL_EFFECT_APPLY_AREA_AURA_PET:
            {
                switch (spell->EffectApplyAuraName[i])
                {
                    case SPELL_AURA_PERIODIC_HEAL:
                        return true;
                }
                break;
            }
        }
    }
    return false;
}

bool IsAlliance(uint8 race)
{
    return race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF ||
#ifndef MANGOSBOT_ZERO
        race == RACE_DRAENEI ||
#endif
            race == RACE_GNOME;
}

/*
enum BotTypeNumber
{
    GROUPER_TYPE_NUMBER = 1,
    ACTIVITY_TYPE_NUMBER = 2
};
*/

uint32 PlayerbotAI::GetFixedBotNumer(BotTypeNumber typeNumber, uint32 maxNum, float cyclePerMin)
{
    uint8 seedNumber = uint8(typeNumber);
    std::mt19937 rng(seedNumber);
    uint32 randseed = rng();                                       //Seed random number
    uint32 randnum = bot->GetGUIDLow() + randseed;                 //Semi-random but fixed number for each bot.

    if (cyclePerMin > 0)
    {
        uint32 cycle = floor(WorldTimer::getMSTime() / (1000));    //Semi-random number adds 1 each second.
        cycle = cycle * cyclePerMin / 60;                          //Cycles cyclePerMin per minute.
        randnum += cycle;                                          //Make the random number cylce.
    }
    randnum = (randnum % (maxNum+1));                              //Loops the randomnumber at maxNum. Bassically removes all the numbers above 99. 
    return randnum;                                                //Now we have a number unique for each bot between 0 and maxNum that increases by cyclePerMin.
}

GrouperType PlayerbotAI::GetGrouperType()
{
    uint32 maxGroupType = sPlayerbotAIConfig.randomBotRaidNearby ? 100 : 95;
    uint32 grouperNumber = GetFixedBotNumer(BotTypeNumber::GROUPER_TYPE_NUMBER, maxGroupType, 0);

    //20% solo
    //50% member
    //20% leader
    //10% raider

    if (grouperNumber < 20 && !HasRealPlayerMaster())
        return GrouperType::SOLO;
    if (grouperNumber < 75)
        return GrouperType::MEMBER;
    if (grouperNumber < 80 || bot->GetLevel() < 3)
        return GrouperType::LEADER_2;
    if (grouperNumber < 85 || bot->GetLevel() < 5)
        return GrouperType::LEADER_3;
    if (grouperNumber < 90 || bot->GetLevel() < 7)
        return GrouperType::LEADER_4;
    if (grouperNumber <= 95 || bot->GetLevel() < 9)
        return GrouperType::LEADER_5;
#ifdef MANGOSBOT_ZERO
    if (grouperNumber <= 97)
        return GrouperType::RAIDER_20;
#else
    if (grouperNumber <= 97)
        return GrouperType::RAIDER_10;
#endif    
   return GrouperType::RAIDER_MAX;
}

GuilderType PlayerbotAI::GetGuilderType()
{
    uint32 grouperNumber = GetFixedBotNumer(BotTypeNumber::GUILDER_TYPE_NUMBER, 100, 0);

    if (grouperNumber < 20 && !HasRealPlayerMaster())
        return GuilderType::SOLO;
    if (grouperNumber < 30)
        return GuilderType::TINY;
    if (grouperNumber < 40)
        return GuilderType::SMALL;
    if (grouperNumber < 60)
        return GuilderType::MEDIUM;
    if (grouperNumber < 80)
        return GuilderType::LARGE;

    return GuilderType::HUGE;
}

bool PlayerbotAI::HasPlayerNearby(WorldPosition pos, float range)
{
    if (!range)
        range = pos.getVisibilityDistance();

    float sqRange = range * range;
    bool nearPlayer = false;
    for (auto& i : sRandomPlayerbotMgr.GetPlayers())
    {
        Player* player = i.second;
        if (!player->IsGameMaster() || player->isGMVisible())
        {
            if (player->GetMapId() != bot->GetMapId())
                continue;

            if (pos.sqDistance(WorldPosition(player)) < sqRange)
                nearPlayer = true;

            // if player is far check farsight/cinematic camera
            Camera& viewPoint = player->GetCamera();
            WorldObject* viewObj = viewPoint.GetBody();
            if (viewObj && viewObj != player)
            {
                if (pos.sqDistance(WorldPosition(viewObj)) < sqRange)
                    nearPlayer = true;
            }
        }
    }

    return nearPlayer;
}

bool PlayerbotAI::HasPlayerNearby(float range)
{ 
    return HasPlayerNearby(bot, range); 
}

bool PlayerbotAI::HasManyPlayersNearby(uint32 trigerrValue, float range)
{
    float sqRange = range * range;
    uint32 found = 0;

    for (auto& i : sRandomPlayerbotMgr.GetPlayers())
    {
        Player* player = i.second;
        if ((!player->IsGameMaster() || player->isGMVisible()) && sServerFacade.GetDistance2d(player, bot) < sqRange)
        {
            found++;

            if (found >= trigerrValue)
                return true;
        }
    }
    return false;
}

/*
enum ActivityType
{
    GRIND_ACTIVITY = 1,
    RPG_ACTIVITY = 2,
    TRAVEL_ACTIVITY = 3,
    OUT_OF_PARTY_ACTIVITY = 4,
    PACKET_ACTIVITY = 5,
    DETAILED_MOVE_ACTIVITY = 6,
    PARTY_ACTIVITY = 7
    ALL_ACTIVITY = 8
};

   General function to check if a bot is allowed to be active or not.
   This function should be checked first before doing heavy-workload.


*/

ActivePiorityType PlayerbotAI::GetPriorityType()
{
    //Has player master. Always active.
    if (HasRealPlayerMaster())
        return ActivePiorityType::HAS_REAL_PLAYER_MASTER;

    //Self bot in a group with a bot master.
    if (IsRealPlayer())
        return ActivePiorityType::IS_REAL_PLAYER;

    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->getSource();

            if (!member || !member->IsInWorld())
                continue;

            if (member == bot)
                continue;

            if (!member->GetPlayerbotAI() || (member->GetPlayerbotAI() && member->GetPlayerbotAI()->HasRealPlayerMaster()))
                return ActivePiorityType::IN_GROUP_WITH_REAL_PLAYER;
        }
    }

    if (bot->IsBeingTeleported()) //Allow activity while teleportation.
        return ActivePiorityType::IN_INSTANCE;

    if (!WorldPosition(bot).isOverworld()) 
        return ActivePiorityType::IN_INSTANCE;

    if (HasPlayerNearby()) 
        return ActivePiorityType::VISIBLE_FOR_PLAYER;

    if (sServerFacade.IsInCombat(bot))
        return ActivePiorityType::IN_COMBAT;

    if (HasPlayerNearby(WorldPosition(bot).getVisibilityDistance() + sPlayerbotAIConfig.reactDistance)) 
        return ActivePiorityType::NEARBY_PLAYER;

    if (sPlayerbotAIConfig.IsFreeAltBot(bot) || HasStrategy("travel once", BotState::BOT_STATE_NON_COMBAT))
        return ActivePiorityType::IS_ALWAYS_ACTIVE;

    if (bot->InBattleGroundQueue()) 
        return ActivePiorityType::IN_BG_QUEUE;

    bool isLFG = false;
#ifdef MANGOSBOT_TWO
    /*if (group)
    {
    if (sLFGMgr.GetQueueInfo(group->GetObjectGuid()))
    {
    isLFG = true;
    }
    }
    if (sLFGMgr.GetQueueInfo(bot->GetObjectGuid()))
    {
    isLFG = true;
    }*/
#endif

    if (isLFG)
        return ActivePiorityType::IN_LFG;

    //If has real players - slow down continents without player
    //This means we first disable bots in a different continent/area.
    if (sRandomPlayerbotMgr.GetPlayers().empty())
        return ActivePiorityType::IN_EMPTY_SERVER;

    // friends always active
    for (auto& i : sRandomPlayerbotMgr.GetPlayers())
    {
        Player* player = i.second;
        if (!player || !player->IsInWorld())
            continue;

        if (player->GetSocial()->HasFriend(bot->GetObjectGuid()))
            return ActivePiorityType::PLAYER_FRIEND;
    }

    // real guild always active if member+
    if (IsInRealGuild())
        return ActivePiorityType::PLAYER_GUILD;

    if (bot->IsBeingTeleported() || !bot->IsInWorld() || !bot->GetMap()->HasRealPlayers())
        return ActivePiorityType::IN_INACTIVE_MAP;

    ContinentArea currentArea = sMapMgr.GetContinentInstanceId(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY());
    if (currentArea == MAP_NO_AREA)
        return ActivePiorityType::IN_ACTIVE_MAP;
    if (!bot->GetMap()->HasActiveAreas(currentArea))
        return ActivePiorityType::IN_ACTIVE_MAP;

    return ActivePiorityType::IN_ACTIVE_AREA;
}

//Returns the lower and upper bracket for bots to be active.
//Ie. 10,20 means all bots in this bracket will be inactive below 10% activityMod, all bots in this bracket will be active above 20% activityMod and scale between those values.
pair<uint32, uint32> PlayerbotAI::GetPriorityBracket(ActivePiorityType type)
{
    switch (type)
    {
    case ActivePiorityType::HAS_REAL_PLAYER_MASTER:
    case ActivePiorityType::IS_REAL_PLAYER:
    case ActivePiorityType::IN_GROUP_WITH_REAL_PLAYER:
    case ActivePiorityType::IN_INSTANCE:
    case ActivePiorityType::VISIBLE_FOR_PLAYER:
        return { 0,0 };
    case ActivePiorityType::IS_ALWAYS_ACTIVE:
    case ActivePiorityType::IN_COMBAT:
        return { 0,10 };
    case ActivePiorityType::IN_BG_QUEUE:
        return { 0,20 };
    case ActivePiorityType::IN_LFG:
        return { 0,30 };
    case ActivePiorityType::NEARBY_PLAYER:
        return { 0,40 };
    case ActivePiorityType::PLAYER_FRIEND:
    case ActivePiorityType::PLAYER_GUILD:
        return { 0,50 };
    case ActivePiorityType::IN_ACTIVE_AREA:
    case ActivePiorityType::IN_EMPTY_SERVER:
        return { 50,100 };
    case ActivePiorityType::IN_ACTIVE_MAP:
        return { 70,100 };
    case ActivePiorityType::IN_INACTIVE_MAP:
        return { 80,100 };
    default :
        return { 90, 100 };
    }

    return { 90, 100 };
}

bool PlayerbotAI::AllowActive(ActivityType activityType)
{
    ActivePiorityType type = GetPriorityType();

    if (activityType == DETAILED_MOVE_ACTIVITY)
    {
        switch (type)
        {
        case ActivePiorityType::HAS_REAL_PLAYER_MASTER:
        case ActivePiorityType::IS_REAL_PLAYER:
        case ActivePiorityType::IN_GROUP_WITH_REAL_PLAYER:
        case ActivePiorityType::IN_INSTANCE:
        case ActivePiorityType::VISIBLE_FOR_PLAYER:
        case ActivePiorityType::IN_COMBAT:
        case ActivePiorityType::NEARBY_PLAYER:
            return true;
            break;
        case ActivePiorityType::IS_ALWAYS_ACTIVE:
        case ActivePiorityType::IN_BG_QUEUE:
        case ActivePiorityType::IN_LFG:
        case ActivePiorityType::PLAYER_FRIEND:
        case ActivePiorityType::PLAYER_GUILD:
        case ActivePiorityType::IN_ACTIVE_AREA:
        case ActivePiorityType::IN_EMPTY_SERVER:
        case ActivePiorityType::IN_ACTIVE_MAP:
        case ActivePiorityType::IN_INACTIVE_MAP:
        default:
            break;
        }
    }
    else if (activityType == REACT_ACTIVITY)
    {
        switch (type)
        {
        case ActivePiorityType::HAS_REAL_PLAYER_MASTER:
        case ActivePiorityType::IS_REAL_PLAYER:
        case ActivePiorityType::IN_GROUP_WITH_REAL_PLAYER:
        case ActivePiorityType::IN_INSTANCE:
        case ActivePiorityType::VISIBLE_FOR_PLAYER:
        case ActivePiorityType::IS_ALWAYS_ACTIVE:
        case ActivePiorityType::IN_COMBAT:
            return true;
            break;
        case ActivePiorityType::NEARBY_PLAYER:
        case ActivePiorityType::IN_BG_QUEUE:
        case ActivePiorityType::IN_LFG:
        case ActivePiorityType::PLAYER_FRIEND:
        case ActivePiorityType::PLAYER_GUILD:
        case ActivePiorityType::IN_ACTIVE_AREA:
        case ActivePiorityType::IN_EMPTY_SERVER:
        case ActivePiorityType::IN_ACTIVE_MAP:
        case ActivePiorityType::IN_INACTIVE_MAP:
        default:
            return false;
            break;
        }
    }

    pair<uint8, uint8> priorityBracket = GetPriorityBracket(type);
   
    float activityPercentage = sRandomPlayerbotMgr.getActivityPercentage(); //Activity between 0 and 100.

    if (!priorityBracket.second) //No scaling
        return true;

    if (priorityBracket.first >= activityPercentage)
        return false;
    if (priorityBracket.second <= activityPercentage && priorityBracket.second < 100)
        return true;

    float activePerc = (activityPercentage - priorityBracket.first) / (priorityBracket.second - priorityBracket.first);

    activePerc *= (priorityBracket.second == 100) ? sPlayerbotAIConfig.botActiveAlone : 100;

    uint32 ActivityNumber = GetFixedBotNumer(BotTypeNumber::ACTIVITY_TYPE_NUMBER, 100, activePerc * 0.01f); //The last number if the amount it cycles per min. Currently set to 1% of the active bots.

    return ActivityNumber <= (activePerc);           //The given percentage of bots should be active and rotate 1% of those active bots each minute.
}

bool PlayerbotAI::AllowActivity(ActivityType activityType, bool checkNow)
{
    if (!allowActiveCheckTimer[activityType])
        allowActiveCheckTimer[activityType] = time(NULL);

    if (!checkNow && time(NULL) < (allowActiveCheckTimer[activityType] + 5))
        return allowActive[activityType];

    bool allowed = AllowActive(activityType);
    allowActive[activityType] = allowed;
    allowActiveCheckTimer[activityType] = time(NULL);
    return allowed;
}

bool PlayerbotAI::IsOpposing(Player* player)
{
    return IsOpposing(player->getRace(), bot->getRace());
}

bool PlayerbotAI::IsOpposing(uint8 race1, uint8 race2)
{
    return (IsAlliance(race1) && !IsAlliance(race2)) || (!IsAlliance(race1) && IsAlliance(race2));
}

void PlayerbotAI::RemoveShapeshift()
{
    RemoveAura("bear form");
    RemoveAura("dire bear form");
    RemoveAura("moonkin form");
    RemoveAura("travel form");
    RemoveAura("cat form");
    RemoveAura("flight form");
    RemoveAura("swift flight form");
    RemoveAura("aquatic form");
    RemoveAura("ghost wolf");
    RemoveAura("tree of life");
}

uint32 PlayerbotAI::GetEquipGearScore(Player* player, bool withBags, bool withBank)
{
    std::vector<uint32> gearScore(EQUIPMENT_SLOT_END);
    uint32 twoHandScore = 0;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            _fillGearScoreData(player, item, &gearScore, twoHandScore);
    }

    if (withBags)
    {
        // check inventory
        for (int i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        {
            if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                _fillGearScoreData(player, item, &gearScore, twoHandScore);
        }

        // check bags
        for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        {
            if (Bag* pBag = (Bag*)player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                {
                    if (Item* item2 = pBag->GetItemByPos(j))
                        _fillGearScoreData(player, item2, &gearScore, twoHandScore);
                }
            }
        }
    }

    if (withBank)
    {
        for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; ++i)
        {
            if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                _fillGearScoreData(player, item, &gearScore, twoHandScore);
        }

        for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
        {
            if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (item->IsBag())
                {
                    Bag* bag = (Bag*)item;
                    for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                    {
                        if (Item* item2 = bag->GetItemByPos(j))
                            _fillGearScoreData(player, item2, &gearScore, twoHandScore);
                    }
                }
            }
        }
    }

    uint8 count = EQUIPMENT_SLOT_END - 2;   // ignore body and tabard slots
    uint32 sum = 0;

    // check if 2h hand is higher level than main hand + off hand
    if (gearScore[EQUIPMENT_SLOT_MAINHAND] + gearScore[EQUIPMENT_SLOT_OFFHAND] < twoHandScore * 2)
    {
        gearScore[EQUIPMENT_SLOT_OFFHAND] = 0;  // off hand is ignored in calculations if 2h weapon has higher score
        --count;
        gearScore[EQUIPMENT_SLOT_MAINHAND] = twoHandScore;
    }

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
       sum += gearScore[i];
    }

    if (count)
    {
        uint32 res = uint32(sum / count);
        return res;
    }
    else
        return 0;
}

uint32 PlayerbotAI::GetEquipStatsValue(Player* player)
{
    uint32 statsValue = 0;
    uint32 specId = sRandomItemMgr.GetPlayerSpecId(player);

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            statsValue += sRandomItemMgr.GetStatWeight(item->GetProto()->ItemId, specId);
    }

    return statsValue;
}

void PlayerbotAI::_fillGearScoreData(Player *player, Item* item, std::vector<uint32>* gearScore, uint32& twoHandScore)
{
    if (!item)
        return;

    if (player->CanUseItem(item->GetProto()) != EQUIP_ERR_OK)
        return;

    uint8 type   = item->GetProto()->InventoryType;
    uint32 level = item->GetProto()->ItemLevel;

    switch (type)
    {
        case INVTYPE_2HWEAPON:
            twoHandScore = std::max(twoHandScore, level);
            break;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
            (*gearScore)[SLOT_MAIN_HAND] = std::max((*gearScore)[SLOT_MAIN_HAND], level);
            break;
        case INVTYPE_SHIELD:
        case INVTYPE_WEAPONOFFHAND:
        case INVTYPE_HOLDABLE:
            (*gearScore)[EQUIPMENT_SLOT_OFFHAND] = std::max((*gearScore)[EQUIPMENT_SLOT_OFFHAND], level);
            break;
        case INVTYPE_THROWN:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_RANGED:
        case INVTYPE_QUIVER:
        case INVTYPE_RELIC:
            (*gearScore)[EQUIPMENT_SLOT_RANGED] = std::max((*gearScore)[EQUIPMENT_SLOT_RANGED], level);
            break;
        case INVTYPE_HEAD:
            (*gearScore)[EQUIPMENT_SLOT_HEAD] = std::max((*gearScore)[EQUIPMENT_SLOT_HEAD], level);
            break;
        case INVTYPE_NECK:
            (*gearScore)[EQUIPMENT_SLOT_NECK] = std::max((*gearScore)[EQUIPMENT_SLOT_NECK], level);
            break;
        case INVTYPE_SHOULDERS:
            (*gearScore)[EQUIPMENT_SLOT_SHOULDERS] = std::max((*gearScore)[EQUIPMENT_SLOT_SHOULDERS], level);
            break;
        case INVTYPE_BODY:
            (*gearScore)[EQUIPMENT_SLOT_BODY] = std::max((*gearScore)[EQUIPMENT_SLOT_BODY], level);
            break;
        case INVTYPE_CHEST:
        case INVTYPE_ROBE:
            (*gearScore)[EQUIPMENT_SLOT_CHEST] = std::max((*gearScore)[EQUIPMENT_SLOT_CHEST], level);
            break;
        case INVTYPE_WAIST:
            (*gearScore)[EQUIPMENT_SLOT_WAIST] = std::max((*gearScore)[EQUIPMENT_SLOT_WAIST], level);
            break;
        case INVTYPE_LEGS:
            (*gearScore)[EQUIPMENT_SLOT_LEGS] = std::max((*gearScore)[EQUIPMENT_SLOT_LEGS], level);
            break;
        case INVTYPE_FEET:
            (*gearScore)[EQUIPMENT_SLOT_FEET] = std::max((*gearScore)[EQUIPMENT_SLOT_FEET], level);
            break;
        case INVTYPE_WRISTS:
            (*gearScore)[EQUIPMENT_SLOT_WRISTS] = std::max((*gearScore)[EQUIPMENT_SLOT_WRISTS], level);
            break;
        case INVTYPE_HANDS:
            (*gearScore)[EQUIPMENT_SLOT_HANDS] = std::max((*gearScore)[EQUIPMENT_SLOT_HANDS], level);
            break;
        // equipped gear score check uses both rings and trinkets for calculation, assume that for bags/banks it is the same
        // with keeping second highest score at second slot
        case INVTYPE_FINGER:
        {
            if ((*gearScore)[EQUIPMENT_SLOT_FINGER1] < level)
            {
                (*gearScore)[EQUIPMENT_SLOT_FINGER2] = (*gearScore)[EQUIPMENT_SLOT_FINGER1];
                (*gearScore)[EQUIPMENT_SLOT_FINGER1] = level;
            }
            else if ((*gearScore)[EQUIPMENT_SLOT_FINGER2] < level)
                (*gearScore)[EQUIPMENT_SLOT_FINGER2] = level;
            break;
        }
        case INVTYPE_TRINKET:
        {
            if ((*gearScore)[EQUIPMENT_SLOT_TRINKET1] < level)
            {
                (*gearScore)[EQUIPMENT_SLOT_TRINKET2] = (*gearScore)[EQUIPMENT_SLOT_TRINKET1];
                (*gearScore)[EQUIPMENT_SLOT_TRINKET1] = level;
            }
            else if ((*gearScore)[EQUIPMENT_SLOT_TRINKET2] < level)
                (*gearScore)[EQUIPMENT_SLOT_TRINKET2] = level;
            break;
        }
        case INVTYPE_CLOAK:
            (*gearScore)[EQUIPMENT_SLOT_BACK] = std::max((*gearScore)[EQUIPMENT_SLOT_BACK], level);
            break;
        default:
            break;
    }
}

string PlayerbotAI::BotStateToString(BotState state)
{
    switch (state)
    {
        case BotState::BOT_STATE_COMBAT: return "Combat";
        case BotState::BOT_STATE_NON_COMBAT: return "Non Combat";
        case BotState::BOT_STATE_DEAD: return "Dead";
        case BotState::BOT_STATE_REACTION: return "Reaction";
        default: return "";
    }
}

string PlayerbotAI::HandleRemoteCommand(string command)
{
    if (command == "state")
    {
        switch (currentState)
        {
        case BotState::BOT_STATE_COMBAT:
            return "combat";
        case BotState::BOT_STATE_DEAD:
            return "dead";
        case BotState::BOT_STATE_NON_COMBAT:
            return "non-combat";
        default:
            return "unknown";
        }
    }
    else if (command == "position")
    {
        ostringstream out; out << bot->GetPositionX() << " " << bot->GetPositionY() << " " << bot->GetPositionZ() << " " << bot->GetMapId() << " " << bot->GetOrientation();
        uint32 area = sServerFacade.GetAreaId(bot);
        if (const AreaTableEntry* areaEntry = GetAreaEntryByAreaID(area))
        {
            if (AreaTableEntry const* zoneEntry = areaEntry->zone ? GetAreaEntryByAreaID(areaEntry->zone) : areaEntry)
                out << " |" << zoneEntry->area_name[0] << "|";
        }
        return out.str();
    }
    else if (command == "tpos")
    {
        Unit* target = *GetAiObjectContext()->GetValue<Unit*>("current target");
        if (!target) {
            return "";
        }

        ostringstream out; out << target->GetPositionX() << " " << target->GetPositionY() << " " << target->GetPositionZ() << " " << target->GetMapId() << " " << target->GetOrientation();
        return out.str();
    }
    else if (command == "movement")
    {
        LastMovement& data = *GetAiObjectContext()->GetValue<LastMovement&>("last movement");
        ostringstream out; out << data.lastMoveShort.getX() << " " << data.lastMoveShort.getY() << " " << data.lastMoveShort.getZ() << " " << data.lastMoveShort.getMapId() << " " << data.lastMoveShort.getO();
        return out.str();
    }
    else if (command == "target")
    {
        Unit* target = *GetAiObjectContext()->GetValue<Unit*>("current target");
        if (!target) {
            return "";
        }

        return target->GetName();
    }
    else if (command == "hp")
    {
        int pct = (int)((static_cast<float> (bot->GetHealth()) / bot->GetMaxHealth()) * 100);
        ostringstream out; out << pct << "%";

        Unit* target = *GetAiObjectContext()->GetValue<Unit*>("current target");
        if (!target) {
            return out.str();
        }

        pct = (int)((static_cast<float> (target->GetHealth()) / target->GetMaxHealth()) * 100);
        out << " / " << pct << "%";
        return out.str();
    }
    else if (command == "strategy")
    {
        return currentEngine->ListStrategies();
    }
    else if (command == "action")
    {
        return currentEngine->GetLastAction();
    }
    else if (command == "values")
    {
        return GetAiObjectContext()->FormatValues();
    }
    else if (command == "travel")
    {
        ostringstream out;

        TravelTarget* target = GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();
        if (target->getDestination()) {
            out << "Destination = " << target->getDestination()->getName();

            out << ": " << target->getDestination()->getTitle();

            if (!(*target->getPosition() == WorldPosition()))
            {
                out << "(" << target->getPosition()->getAreaName() << ")";
                out << " distance: " << target->getPosition()->distance(bot) << "y";
            }
        }
        out << " Status =";
        if (target->getStatus() == TravelStatus::TRAVEL_STATUS_NONE)
            out << " none";
        else if (target->getStatus() == TravelStatus::TRAVEL_STATUS_PREPARE)
            out << " prepare";
        else if (target->getStatus() == TravelStatus::TRAVEL_STATUS_TRAVEL)
            out << " travel";
        else if (target->getStatus() == TravelStatus::TRAVEL_STATUS_WORK)
            out << " work";
        else if (target->getStatus() == TravelStatus::TRAVEL_STATUS_COOLDOWN)
            out << " cooldown";
        else if (target->getStatus() == TravelStatus::TRAVEL_STATUS_EXPIRED)
            out << " expired";

        if(target->getStatus() != TravelStatus::TRAVEL_STATUS_EXPIRED)
            out << " Expire in " << (target->getTimeLeft()/1000) << "s";

        out << " Retry " << target->getRetryCount(true) << "/" << target->getRetryCount(false);

        return out.str();
    }
    else if (command == "budget")
    {
        ostringstream out;

        AiObjectContext* context = GetAiObjectContext();

        out << "Current money: " << ChatHelper::formatMoney(bot->GetMoney()) << " free to use:" << ChatHelper::formatMoney(AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::anything)) << "\n";
        out << "Purpose | Available / Needed \n";

        for (uint32 i = 1; i < (uint32)NeedMoneyFor::anything; i++)
        {
            NeedMoneyFor needMoneyFor = NeedMoneyFor(i);

            switch (needMoneyFor)
            {
            case NeedMoneyFor::none:
                out << "nothing";
                break;
            case NeedMoneyFor::repair:
                out << "repair";
                break;
            case NeedMoneyFor::ammo:
                out << "ammo";
                break;
            case NeedMoneyFor::spells:
                out << "spells";
                break;
            case NeedMoneyFor::travel:
                out << "travel";
                break;
            case NeedMoneyFor::consumables:
                out << "consumables";
                break;
            case NeedMoneyFor::gear:
                out << "gear";
                break;
            case NeedMoneyFor::guild:
                out << "guild";
                break;
            }
            out << " | " << ChatHelper::formatMoney(AI_VALUE2(uint32, "free money for", i)) << " / " << ChatHelper::formatMoney(AI_VALUE2(uint32, "money needed for", i)) << "\n";
        }

        return out.str();
    }
    ostringstream out; out << "invalid command: " << command;
    return out.str();
}

bool PlayerbotAI::HasSkill(SkillType skill)
{
    return bot->HasSkill(skill) && bot->GetSkillValue(skill) > 0;
}

bool ChatHandler::HandlePlayerbotCommand(char* args)
{
    return PlayerbotMgr::HandlePlayerbotMgrCommand(this, args);
}

bool ChatHandler::HandleRandomPlayerbotCommand(char* args)
{
    return RandomPlayerbotMgr::HandlePlayerbotConsoleCommand(this, args);
}

bool ChatHandler::HandleAhBotCommand(char* args)
{
    return ahbot::AhBot::HandleAhBotCommand(this, args);
}

bool ChatHandler::HandleGuildTaskCommand(char* args)
{
    return GuildTaskMgr::HandleConsoleCommand(this, args);
}

float PlayerbotAI::GetRange(string type)
{
    float val = 0;

    if (type == "follow" && bot->GetGroup() && bot->GetGroup()->IsRaidGroup())
        type = "followraid";

    if (aiObjectContext) val = aiObjectContext->GetValue<float>("range", type)->Get();
    if (abs(val) >= 0.1f) return val;

    if (type == "spell") return sPlayerbotAIConfig.spellDistance;
    if (type == "shoot") return sPlayerbotAIConfig.shootDistance;
    if (type == "flee") return sPlayerbotAIConfig.fleeDistance;
    if (type == "heal") return sPlayerbotAIConfig.healDistance;
    if (type == "follow") return sPlayerbotAIConfig.followDistance;    
    if (type == "followraid") return sPlayerbotAIConfig.raidFollowDistance;
    if (type == "attack") return 0;
    return 0;
}

//Copy from reputation GetFactionReaction
ReputationRank PlayerbotAI::GetFactionReaction(FactionTemplateEntry const* thisTemplate, FactionTemplateEntry const* otherTemplate)
{
    MANGOS_ASSERT(thisTemplate)
        MANGOS_ASSERT(otherTemplate)

        // Original logic begins

        if (otherTemplate->factionGroupMask & thisTemplate->enemyGroupMask)
            return REP_HOSTILE;

    if (thisTemplate->enemyFaction[0] && otherTemplate->faction)
    {
        for (unsigned int i : thisTemplate->enemyFaction)
        {
            if (i == otherTemplate->faction)
                return REP_HOSTILE;
        }
    }

    if (otherTemplate->factionGroupMask & thisTemplate->friendGroupMask)
        return REP_FRIENDLY;

    if (thisTemplate->friendFaction[0] && otherTemplate->faction)
    {
        for (unsigned int i : thisTemplate->friendFaction)
        {
            if (i == otherTemplate->faction)
                return REP_FRIENDLY;
        }
    }

    if (thisTemplate->factionGroupMask & otherTemplate->friendGroupMask)
        return REP_FRIENDLY;

    if (otherTemplate->friendFaction[0] && thisTemplate->faction)
    {
        for (unsigned int i : otherTemplate->friendFaction)
        {
            if (i == thisTemplate->faction)
                return REP_FRIENDLY;
        }
    }
    return REP_NEUTRAL;
}

bool PlayerbotAI::AddAura(Unit* unit, uint32 spellId)
{
    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form    

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;

    if (!IsSpellAppliesAura(spellInfo, (1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) &&
        !IsSpellHaveEffect(spellInfo, SPELL_EFFECT_PERSISTENT_AREA_AURA))
    {
        return false;
    }

    SpellAuraHolder* holder = CreateSpellAuraHolder(spellInfo, unit, unit);

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        uint8 eff = spellInfo->Effect[i];
        if (eff >= MAX_SPELL_EFFECTS)
            continue;
        if (IsAreaAuraEffect(eff) ||
            eff == SPELL_EFFECT_APPLY_AURA ||
            eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        {
            int32 basePoints = spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
            int32 damage = basePoints;
            Aura* aur = CreateAura(spellInfo, SpellEffectIndex(i), &damage, &basePoints, holder, unit);
            holder->AddAura(aur, SpellEffectIndex(i));
        }
    }
    if (!unit->AddSpellAuraHolder(holder))
        delete holder;

    return true;
}

void PlayerbotAI::InventoryIterateItems(IterateItemsVisitor* visitor, IterateItemsMask mask)
{
    if ((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_BAGS)
        InventoryIterateItemsInBags(visitor);

    if ((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_EQUIP)
        InventoryIterateItemsInEquip(visitor);

    if ((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_BANK)
        InventoryIterateItemsInBank(visitor);

    if ((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_BUYBACK)
        InventoryIterateItemsInBuyBack(visitor);
}

void PlayerbotAI::InventoryIterateItemsInBags(IterateItemsVisitor* visitor)
{
    for (int i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (Item* pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            if (!visitor->Visit(pItem))
                return;

    for (int i = KEYRING_SLOT_START; i < KEYRING_SLOT_END; ++i)
        if (Item* pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            if (!visitor->Visit(pItem))
                return;

    for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        if (Bag* pBag = (Bag*)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                if (Item* pItem = pBag->GetItemByPos(j))
                    if (!visitor->Visit(pItem))
                        return;
}

void PlayerbotAI::InventoryIterateItemsInEquip(IterateItemsVisitor* visitor)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
    {
        Item* pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        if (!visitor->Visit(pItem))
            return;
    }
}

void PlayerbotAI::InventoryIterateItemsInBank(IterateItemsVisitor* visitor)
{
    for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_END; slot++)
    {
        Item* pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        if (!visitor->Visit(pItem))
            return;
    }

    for (int i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
    {
        if (Bag* pBag = (Bag*)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            if (pBag)
            {
                for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                {
                    if (Item* pItem = pBag->GetItemByPos(j))
                    {
                        if (!pItem)
                            continue;

                        if (!visitor->Visit(pItem))
                            return;
                    }
                }
            }
        }
    }

}

void PlayerbotAI::InventoryIterateItemsInBuyBack(IterateItemsVisitor* visitor)
{
    for (uint8 slot = BUYBACK_SLOT_START; slot < BUYBACK_SLOT_END; slot++)
    {
        Item* pItem = bot->GetItemFromBuyBackSlot(slot);
        if (!pItem)
            continue;

        if (!visitor->Visit(pItem))
            return;
    }
}

bool compare_items(const ItemPrototype* proto1, const ItemPrototype* proto2)
{
    if (proto1->Class != proto2->Class)
        return proto1->Class > proto2->Class;

    if (proto1->SubClass != proto2->SubClass)
        return proto1->SubClass < proto2->SubClass;

    if (proto1->Quality != proto2->Quality)
        return proto1->Quality < proto2->Quality;

    if (proto1->ItemLevel != proto2->ItemLevel)
        return proto1->ItemLevel > proto2->ItemLevel;

    return false;
}

bool compare_items_by_level(const Item* item1, const Item* item2)
{
    return compare_items(item1->GetProto(), item2->GetProto());
}

void PlayerbotAI::InventoryTellItems(Player* player, map<uint32, int> itemMap, map<uint32, bool> soulbound)
{
    list<ItemPrototype const*> items;
    for (map<uint32, int>::iterator i = itemMap.begin(); i != itemMap.end(); i++)
    {
        items.push_back(sObjectMgr.GetItemPrototype(i->first));
    }

    items.sort(compare_items);

    uint32 oldClass = -1;
    for (list<ItemPrototype const*>::iterator i = items.begin(); i != items.end(); i++)
    {
        ItemPrototype const* proto = *i;

        if (proto->Class != oldClass)
        {
            oldClass = proto->Class;
            switch (proto->Class)
            {
            case ITEM_CLASS_CONSUMABLE:
                TellPlayer(player, "--- consumable ---");
                break;
            case ITEM_CLASS_CONTAINER:
                TellPlayer(player, "--- container ---");
                break;
            case ITEM_CLASS_WEAPON:
                TellPlayer(player, "--- weapon ---");
                break;
            case ITEM_CLASS_ARMOR:
                TellPlayer(player, "--- armor ---");
                break;
            case ITEM_CLASS_REAGENT:
                TellPlayer(player, "--- reagent ---");
                break;
            case ITEM_CLASS_PROJECTILE:
                TellPlayer(player, "--- projectile ---");
                break;
            case ITEM_CLASS_TRADE_GOODS:
                TellPlayer(player, "--- trade goods ---");
                break;
            case ITEM_CLASS_RECIPE:
                TellPlayer(player, "--- recipe ---");
                break;
            case ITEM_CLASS_QUIVER:
                TellPlayer(player, "--- quiver ---");
                break;
            case ITEM_CLASS_QUEST:
                TellPlayer(player, "--- quest items ---");
                break;
            case ITEM_CLASS_KEY:
                TellPlayer(player, "--- keys ---");
                break;
            case ITEM_CLASS_MISC:
                TellPlayer(player, "--- other ---");
                break;
            }
        }

        InventoryTellItem(player, proto, itemMap[proto->ItemId], soulbound[proto->ItemId]);
    }
}

void PlayerbotAI::InventoryTellItem(Player* player, ItemPrototype const* proto, int count, bool soulbound)
{
    ostringstream out;
    out << GetChatHelper()->formatItem(proto, count);
    if (soulbound)
        out << " (soulbound)";
    TellPlayer(player, out.str());
}

#define VISIT_MASK(visitmask) \
    InventoryIterateItems(&visitor, visitmask);\
    found.insert(visitor.GetResult().begin(), visitor.GetResult().end())

#define VISIT \
    InventoryIterateItems(&visitor, mask);\
    found.insert(visitor.GetResult().begin(), visitor.GetResult().end())

#define RETURN_SORT_FOUND \
    list<Item*> result; \
    for (set<Item*>::iterator i = found.begin(); i != found.end(); ++i)\
        result.push_back(*i); \
    result.sort(compare_items_by_level); \
    return result

#define RETURN_FOUND \
    list<Item*> result; \
    for (set<Item*>::iterator i = found.begin(); i != found.end(); ++i)\
        result.push_back(*i); \
    return result


list<Item*> PlayerbotAI::InventoryParseItems(string text, IterateItemsMask mask)
{
    AiObjectContext* context = aiObjectContext;

    set<Item*> found;
    size_t pos = text.find(" ");
    int count = pos != string::npos ? atoi(text.substr(pos + 1).c_str()) : 1;
    if (count < 1) count = 1;

    ItemIds ids = GetChatHelper()->parseItems(text);
    if (!ids.empty())
    {
        for (ItemIds::iterator i = ids.begin(); i != ids.end(); i++)
        {
            FindItemByIdVisitor visitor(*i);
            VISIT;
        }

        RETURN_SORT_FOUND;
    }

    if (text == "all" || text == "*")
    {
        FindAllItemVisitor visitor;
        VISIT_MASK(mask);
        RETURN_SORT_FOUND;
    }
    else if (text == "equip")
    {
        if (!((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_EQUIP))
            return {};

        FindAllItemVisitor visitor;
        VISIT_MASK(IterateItemsMask::ITERATE_ITEMS_IN_EQUIP);
        RETURN_SORT_FOUND;
    }
    else if (text == "inventory")
    {
        if (!((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_BAGS))
            return {};

        FindAllItemVisitor visitor;
        VISIT_MASK(IterateItemsMask::ITERATE_ITEMS_IN_BAGS);
        RETURN_SORT_FOUND;
    }
    else if (text == "bank")
    {
        if (!((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_BANK))
            return {};

        FindAllItemVisitor visitor;
        VISIT_MASK(IterateItemsMask::ITERATE_ITEMS_IN_BANK);
        RETURN_SORT_FOUND;
    }
    else if (text == "buyback")
    {
        if (!((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_BUYBACK))
            return {};

        FindAllItemVisitor visitor;
        VISIT_MASK(IterateItemsMask::ITERATE_ITEMS_IN_BUYBACK);
        RETURN_SORT_FOUND;
    }

    if (text == "food" || text == "conjured food")
    {
        FindFoodVisitor visitor(bot, 11, (text == "conjured food"));
        VISIT;
    }
    else if (text == "drink" || text == "water" || text == "conjured drink" || text == "conjured water")
    {
        FindFoodVisitor visitor(bot, 59, (text == "conjured drink" || text == "conjured water"));
        VISIT;
    }
    else if (text == "mana potion")
    {
        FindPotionVisitor visitor(bot, SPELL_EFFECT_ENERGIZE);
        VISIT;
    }
    else if (text == "healing potion")
    {
        FindPotionVisitor visitor(bot, SPELL_EFFECT_HEAL);
        VISIT;
    }
    else if (text == "mount")
    {
        FindMountVisitor visitor(bot);
        VISIT;
    }
    else if (text == "pet")
    {
        FindPetVisitor visitor(bot);
        VISIT;
    }
    else if (text == "ammo")
    {
        Item* const pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
        if (pItem)
        {
            FindAmmoVisitor visitor(bot, pItem->GetProto()->SubClass);
            VISIT;
        }
    }
    else if (text == "recipe")
    {
        FindRecipeVisitor visitor(bot);
        VISIT;
    }
    else if (text == "quest")
    {
        FindQuestItemVisitor visitor(bot);
        VISIT;
    }
    else if (text.find("usage ") == 0)
    {
        FindItemUsageVisitor visitor(bot, ItemUsage(stoi(text.substr(6))));
        VISIT;
    }
    else if (text == "tradeskill")
    {
        FindItemUsageVisitor visitor(bot, ItemUsage::ITEM_USAGE_SKILL);
        VISIT;
    }
    else if (text == "use")
    {
        FindItemUsageVisitor visitor(bot, ItemUsage::ITEM_USAGE_USE);
        VISIT;
    }
    else if (text == "vendor")
    {
        FindItemUsageVisitor visitor(bot, ItemUsage::ITEM_USAGE_VENDOR);
        VISIT;

        if (AI_VALUE(uint8, "bag space") > 80 && !urand(0, 10))
        {
            FindItemUsageVisitor visitor(bot, ItemUsage::ITEM_USAGE_AH);
            VISIT;
        }
    }

    FindNamedItemVisitor visitor(bot, text);
    VISIT;

    uint32 quality = GetChatHelper()->parseItemQuality(text);
    if (quality != MAX_ITEM_QUALITY)
    {
        FindItemsToTradeByQualityVisitor visitor(quality, count);
        VISIT;
    }

    uint32 itemClass = MAX_ITEM_CLASS, itemSubClass = 0;
    if (GetChatHelper()->parseItemClass(text, &itemClass, &itemSubClass))
    {
        FindItemsByClassVisitor visitor(itemClass, itemSubClass);
        VISIT;
    }

    uint32 fromSlot = GetChatHelper()->parseSlot(text);
    if (fromSlot != EQUIPMENT_SLOT_END)
    {
        if ((uint8)mask & (uint8)IterateItemsMask::ITERATE_ITEMS_IN_EQUIP)
        {
            Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, fromSlot);
            if (item)
                found.insert(item);
        }

        FindItemBySlotVisitor visitor(bot, fromSlot);
        VISIT;
    }

    ItemIds outfit = InventoryFindOutfitItems(text);
    if (!outfit.empty())
    {
        FindItemByIdsVisitor visitor(outfit);
        VISIT;
    }

    RETURN_SORT_FOUND;
}

uint32 PlayerbotAI::InventoryGetItemCount(FindItemVisitor* visitor, IterateItemsMask mask)
{
    InventoryIterateItems(visitor, mask);
    uint32 count = 0;
    list<Item*>& items = visitor->GetResult();
    for (list<Item*>::iterator i = items.begin(); i != items.end(); ++i)
    {
        Item* item = *i;
        count += item->GetCount();
    }
    return count;
}

ItemIds PlayerbotAI::InventoryFindOutfitItems(string name)
{
    AiObjectContext* context = GetAiObjectContext();
    list<string>& outfits = AI_VALUE(list<string>&, "outfit list");
    for (list<string>::iterator i = outfits.begin(); i != outfits.end(); ++i)
    {
        string outfit = *i;
        if (name == InventoryParseOutfitName(outfit))
            return InventoryParseOutfitItems(outfit);
    }
    return set<uint32>();
}

void PlayerbotAI::AccelerateRespawn(Creature* creature, float accelMod)
{
    if (!creature)
        return;

    if (!sPlayerbotAIConfig.respawnModForPlayerBots && HasRealPlayerMaster())
        return;

    if (!sPlayerbotAIConfig.respawnModForInstances && !WorldPosition(creature).isOverworld())
        return;

    AiObjectContext* context = aiObjectContext;
    if (!accelMod)
    {
        if (!sPlayerbotAIConfig.respawnModHostile && !sPlayerbotAIConfig.respawnModNeutral)
            return;

        uint32 playersNr = AI_VALUE_LAZY(list<ObjectGuid>, "nearest friendly players").size()+1;
      
     
        if (playersNr <= sPlayerbotAIConfig.respawnModThreshold)
            return;

        playersNr = std::min(playersNr - sPlayerbotAIConfig.respawnModThreshold, sPlayerbotAIConfig.respawnModMax);

        accelMod = playersNr * (creature->CanAttackOnSight(bot) ? sPlayerbotAIConfig.respawnModHostile : sPlayerbotAIConfig.respawnModNeutral) * 0.01f;
    }

    if (!accelMod)
        return;

    uint32 m_respawnDelay;
    uint32 m_corpseAccelerationDecayDelay;

    if (accelMod >= 1) 
    {
        m_respawnDelay = 0;
        m_corpseAccelerationDecayDelay = 0;
    }
    else
    {
        CreatureData const* data = sObjectMgr.GetCreatureData(creature->GetDbGuid());

        if (!data)
            return;

        m_respawnDelay = data->GetRandomRespawnTime() * IN_MILLISECONDS;
        m_corpseAccelerationDecayDelay = MINIMUM_LOOTING_TIME;          

        uint32 totalDelay = m_respawnDelay + m_corpseAccelerationDecayDelay;

        if (m_respawnDelay < totalDelay * accelMod) 
        {
            if ((m_corpseAccelerationDecayDelay- ((totalDelay * accelMod) - m_respawnDelay)) < 0)
                sLog.outError("m_corpseAccelerationDecayDelay: %d, totalDelay: %d, accelMod: %f, m_respawnDelay: %d", m_corpseAccelerationDecayDelay, totalDelay, accelMod, m_respawnDelay);

            m_corpseAccelerationDecayDelay -= (totalDelay * accelMod) - m_respawnDelay;
            m_respawnDelay = 0;
        }
        else
            m_respawnDelay -= totalDelay * accelMod;
    }

    creature->SetRespawnDelay(m_respawnDelay / IN_MILLISECONDS,true);

    if (!m_corpseAccelerationDecayDelay && creature->m_loot)
    {
        LootAccess const* lootAccess = reinterpret_cast<LootAccess const*>(creature->m_loot);

        if (lootAccess->IsLootedForAll()) //No loot left. Just despawn the corpse.
        {
            creature->RemoveCorpse();
            return;
        }

        uint32 defaultDelay = 2 * MINUTE;

        CreatureInfo const* cinfo = creature->GetCreatureInfo();

        if (cinfo->CorpseDelay)
            defaultDelay = cinfo->CorpseDelay;
        else if (sObjectMgr.IsEncounter(creature->GetEntry(), creature->GetMapId()))
        {
            // encounter boss forced decay timer to 1h
            defaultDelay = 3600;                               // TODO: maybe add that to config file
        }
        else
        {
            switch (cinfo->Rank)
            {
            case CREATURE_ELITE_RARE:
                defaultDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_RARE);
                break;
            case CREATURE_ELITE_ELITE:
                defaultDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_ELITE);
                break;
            case CREATURE_ELITE_RAREELITE:
                defaultDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_RAREELITE);
                break;
            case CREATURE_ELITE_WORLDBOSS:
                defaultDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_WORLDBOSS);
                break;
            default:
                defaultDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_NORMAL);
                break;
            }
        }

        defaultDelay *= IN_MILLISECONDS / (1+accelMod);

        //We will decrease the loot time by a factor capping at 20 seconds.
        m_corpseAccelerationDecayDelay = std::max(uint32(20 * IN_MILLISECONDS), defaultDelay);
        creature->SetCorpseAccelerationDelay(m_corpseAccelerationDecayDelay);

        creature->ReduceCorpseDecayTimer();
        return;
    }
    MANGOS_ASSERT(m_corpseAccelerationDecayDelay < 24 * HOUR* IN_MILLISECONDS);
    creature->SetCorpseAccelerationDelay(m_corpseAccelerationDecayDelay);
}

string PlayerbotAI::InventoryParseOutfitName(string outfit)
{
    int pos = outfit.find("=");
    if (pos == -1) return "";
    return outfit.substr(0, pos);
}

ItemIds PlayerbotAI::InventoryParseOutfitItems(string text)
{
    ItemIds itemIds;

    uint8 pos = text.find("=") + 1;
    while (pos < text.size())
    {
        int endPos = text.find(',', pos);
        if (endPos == -1)
            endPos = text.size();

        string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos + 1;
        if (id)
            itemIds.insert(id);
    }

    return itemIds;
}

void PlayerbotAI::Ping(float x, float y)
{
    WorldPacket data(MSG_MINIMAP_PING, (8 + 4 + 4));
    data << bot->GetObjectGuid();
    data << x;
    data << y;

    if (bot->GetGroup())
    {
        bot->GetGroup()->BroadcastPacket(
#ifdef MANGOS
            & data,
#endif
#ifdef CMANGOS
            data,
#endif
            true, -1, bot->GetObjectGuid());
    }
    else
    {
        bot->GetSession()->SendPacket(
#ifdef MANGOS
            & data
#endif
#ifdef CMANGOS
            data
#endif
            );
    }
}

void PlayerbotAI::Poi(float x, float y, string icon_name, Player* player, uint32 flags, uint32 icon, uint32 icon_data)
{
    if (!player)
        player = master;

    if (!player)
        return;

    WorldPacket data(SMSG_GOSSIP_POI, (4 + 4 + 4 + 4 + 4 + 10)); // guess size
    data << flags;
    data << x;
    data << y;
    data << icon;
    data << icon_data;
    data << icon_name;

    sServerFacade.SendPacket(player, data);
}

//Find Poison ...Natsukawa
Item* PlayerbotAI::FindPoison() const
{
   // list out items in main backpack
   for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
   {
      Item* const pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
      if (pItem)
      {
         const ItemPrototype* const pItemProto = pItem->GetProto();

         if (!pItemProto || bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
            continue;

         if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
            return pItem;
      }
   }
   // list out items in other removable backpacks
   for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
   {
      const Bag* const pBag = (Bag *)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
      if (pBag)
         for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
         {
            Item* const pItem = bot->GetItemByPos(bag, slot);
            if (pItem)
            {
               const ItemPrototype* const pItemProto = pItem->GetProto();

               if (!pItemProto || bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                  continue;

               if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                  return pItem;
            }
         }
   }
   return NULL;
}

Item* PlayerbotAI::FindConsumable(uint32 displayId) const
{
   // list out items in main backpack
   for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
   {
      Item* const pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
      if (pItem)
      {
         const ItemPrototype* const pItemProto = pItem->GetProto();

         if (!pItemProto || bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
            continue;

         if ((pItemProto->Class == ITEM_CLASS_CONSUMABLE || pItemProto->Class == ITEM_CLASS_TRADE_GOODS) && pItemProto->DisplayInfoID == displayId)
            return pItem;
      }
   }
   // list out items in other removable backpacks
   for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
   {
      const Bag* const pBag = (Bag *)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
      if (pBag)
         for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
         {
            Item* const pItem = bot->GetItemByPos(bag, slot);
            if (pItem)
            {
               const ItemPrototype* const pItemProto = pItem->GetProto();

               if (!pItemProto || bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                  continue;

               if ((pItemProto->Class == ITEM_CLASS_CONSUMABLE || pItemProto->Class == ITEM_CLASS_TRADE_GOODS) && pItemProto->DisplayInfoID == displayId)
                  return pItem;
            }
         }
   }
   return NULL;
}

Item* PlayerbotAI::FindBandage() const
{
   // list out items in main backpack
   for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
   {
      Item* const pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
      if (pItem)
      {
         const ItemPrototype* const pItemProto = pItem->GetProto();

         if (!pItemProto || bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
            continue;
#ifdef MANGOSBOT_ZERO
         if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
#else
         if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
#endif
            return pItem;
      }
   }
   // list out items in other removable backpacks
   for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
   {
      const Bag* const pBag = (Bag *)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
      if (pBag)
         for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
         {
            Item* const pItem = bot->GetItemByPos(bag, slot);
            if (pItem)
            {
               const ItemPrototype* const pItemProto = pItem->GetProto();

               if (!pItemProto || bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                  continue;

#ifdef MANGOSBOT_ZERO
               if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
#else
               if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
#endif
                  return pItem;
            }
         }
   }
   return nullptr;
}

static const uint32 uPriorizedSharpStoneIds[8] =
{
    ADAMANTITE_SHARPENING_DISPLAYID, FEL_SHARPENING_DISPLAYID, ELEMENTAL_SHARPENING_DISPLAYID, DENSE_SHARPENING_DISPLAYID,
    SOLID_SHARPENING_DISPLAYID, HEAVY_SHARPENING_DISPLAYID, COARSE_SHARPENING_DISPLAYID, ROUGH_SHARPENING_DISPLAYID
};

static const uint32 uPriorizedWeightStoneIds[7] =
{
    ADAMANTITE_WEIGHTSTONE_DISPLAYID, FEL_WEIGHTSTONE_DISPLAYID, DENSE_WEIGHTSTONE_DISPLAYID, SOLID_WEIGHTSTONE_DISPLAYID,
    HEAVY_WEIGHTSTONE_DISPLAYID, COARSE_WEIGHTSTONE_DISPLAYID, ROUGH_WEIGHTSTONE_DISPLAYID
};

/**
 * FindStoneFor()
 * return Item* Returns sharpening/weight stone item eligible to enchant a bot weapon
 *
 * params:weapon Item* the weap�n the function should search and return a enchanting item for
 * return nullptr if no relevant item is found in bot inventory, else return a sharpening or weight
 * stone based on the weapon subclass
 *
 */
Item* PlayerbotAI::FindStoneFor(Item* weapon) const
{
   Item* stone;
   ItemPrototype const* pProto = weapon->GetProto();
   if (pProto && (pProto->SubClass == ITEM_SUBCLASS_WEAPON_SWORD || pProto->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2
      || pProto->SubClass == ITEM_SUBCLASS_WEAPON_AXE || pProto->SubClass == ITEM_SUBCLASS_WEAPON_AXE2
      || pProto->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER))
   {
      for (uint8 i = 0; i < countof(uPriorizedSharpStoneIds); ++i)
      {
         stone = FindConsumable(uPriorizedSharpStoneIds[i]);
         if (stone)
            return stone;
      }
   }
   else if (pProto && (pProto->SubClass == ITEM_SUBCLASS_WEAPON_MACE || pProto->SubClass == ITEM_SUBCLASS_WEAPON_MACE2))
   {
      for (uint8 i = 0; i < countof(uPriorizedWeightStoneIds); ++i)
      {
         stone = FindConsumable(uPriorizedWeightStoneIds[i]);
         if (stone)
            return stone;
      }
   }

   return nullptr;
}

static const uint32 uPriorizedWizardOilIds[5] =
{
    MINOR_WIZARD_OIL, LESSER_WIZARD_OIL, BRILLIANT_WIZARD_OIL, WIZARD_OIL, SUPERIOR_WIZARD_OIL
};

static const uint32 uPriorizedManaOilIds[4] =
{
   MINOR_MANA_OIL, LESSER_MANA_OIL, BRILLIANT_MANA_OIL, SUPERIOR_MANA_OIL,
};

Item* PlayerbotAI::FindOilFor(Item* weapon) const
{
    Item* oil = nullptr;
    ItemPrototype const* pProto = weapon->GetProto();

    const std::vector<uint32> uPriorizedWizardOilIds = { MINOR_WIZARD_OIL, LESSER_WIZARD_OIL, BRILLIANT_WIZARD_OIL, WIZARD_OIL, SUPERIOR_WIZARD_OIL };
    const std::vector<uint32> uPriorizedManaOilIds = { MINOR_MANA_OIL, LESSER_MANA_OIL, BRILLIANT_MANA_OIL, SUPERIOR_MANA_OIL };

    if (pProto && (pProto->SubClass == ITEM_SUBCLASS_WEAPON_SWORD || pProto->SubClass == ITEM_SUBCLASS_WEAPON_STAFF || pProto->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER))
    {
        for (uint8 i = 0; i < uPriorizedWizardOilIds.size(); i++)
        {
            oil = FindConsumable(uPriorizedWizardOilIds[i]);
            if (!oil && i < uPriorizedManaOilIds.size())
            {
                oil = FindConsumable(uPriorizedManaOilIds[i]);
            }

            if(oil)
            {
                break;
            }
        }
    }
    else if (pProto && (pProto->SubClass == ITEM_SUBCLASS_WEAPON_MACE || pProto->SubClass == ITEM_SUBCLASS_WEAPON_MACE2))
    {
        for (uint8 i = 0; i < uPriorizedManaOilIds.size(); i++)
        {
            oil = FindConsumable(uPriorizedManaOilIds[i]);
            if (!oil && i < uPriorizedWizardOilIds.size())
            {
                oil = FindConsumable(uPriorizedWizardOilIds[i]);
            }

            if (oil)
            {
                break;
            }
        }
    }

    return oil;
}

//  on self
void PlayerbotAI::ImbueItem(Item* item)
{
   ImbueItem(item, TARGET_FLAG_SELF, ObjectGuid());
}

//  item on unit
void PlayerbotAI::ImbueItem(Item* item, Unit* target)
{
   if (!target)
      return;

   ImbueItem(item, TARGET_FLAG_UNIT, target->GetObjectGuid());
}

//  item on equipped item
void PlayerbotAI::ImbueItem(Item* item, uint8 targetInventorySlot)
{
   if (targetInventorySlot >= EQUIPMENT_SLOT_END)
      return;

   Item* const targetItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, targetInventorySlot);
   if (!targetItem)
      return;

   ImbueItem(item, TARGET_FLAG_ITEM, targetItem->GetObjectGuid());
}

// generic item use method
#ifdef MANGOSBOT_ZERO
void PlayerbotAI::ImbueItem(Item* item, uint16 targetFlag, ObjectGuid targetGUID)
#else
void PlayerbotAI::ImbueItem(Item* item, uint32 targetFlag, ObjectGuid targetGUID)
#endif
{
   if (!item)
      return;

   uint8 bagIndex = item->GetBagSlot();
   uint8 slot = item->GetSlot();
   uint8 cast_count = 0;
   ObjectGuid item_guid = item->GetObjectGuid();

   uint32 spellId = 0;
   uint8 spell_index = 0;
   for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
   {
      if (item->GetProto()->Spells[i].SpellId > 0)
      {
         spellId = item->GetProto()->Spells[i].SpellId;
         spell_index = i;
         break;
      }
   }

#ifdef CMANGOS
   std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_USE_ITEM, 20));
#endif
#ifdef MANGOS
   WorldPacket* packet = new WorldPacket(CMSG_USE_ITEM);
#endif

   *packet << bagIndex;
   *packet << slot;
   *packet << spell_index;
#ifdef MANGOSBOT_ZERO
   *packet << targetFlag;
#endif
#ifdef MANGOSBOT_ONE
   *packet << cast_count;
   *packet << item_guid;
   *packet << targetFlag;
#endif
#ifdef MANGOSBOT_TWO
   *packet << spellId << item_guid << uint32(0) << uint8(0);
   *packet << targetFlag;
#endif

#ifdef CMANGOS
   if (targetFlag & (TARGET_FLAG_UNIT | TARGET_FLAG_ITEM | TARGET_FLAG_GAMEOBJECT))
#endif
#ifdef MANGOS
   if (targetFlag & (TARGET_FLAG_UNIT | TARGET_FLAG_ITEM | TARGET_FLAG_OBJECT))
#endif
      *packet << targetGUID.WriteAsPacked();

#ifdef CMANGOS
   bot->GetSession()->QueuePacket(std::move(packet));
#endif
#ifdef MANGOS
   bot->GetSession()->QueuePacket(packet);
#endif
}

void PlayerbotAI::EnchantItemT(uint32 spellid, uint8 slot, Item* item)
{
    Item* pItem = nullptr;
    if (item)
        pItem = item;
    else
        pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

   if (!pItem)
    return;

   if (pItem->GetSlot() != slot)
       return;

#ifdef CMANGOS
   if (pItem->GetOwner() == nullptr)
       return;
#endif

#ifdef MANGOS
   SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
#else
   SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellid);
#endif
   if (!spellInfo)
      return;

   uint32 enchantid = spellInfo->EffectMiscValue[0];
   if (!enchantid)
   {
      sLog.outError("%u: Invalid enchantid %s %s" , enchantid , " report to devs", bot->GetName());
      return;
   }

   if (!((1 << pItem->GetProto()->SubClass) & spellInfo->EquippedItemSubClassMask) &&
      !((1 << pItem->GetProto()->InventoryType) & spellInfo->EquippedItemInventoryTypeMask))
   {
      
      sLog.outError("%s: items could not be enchanted, wrong item type equipped", bot->GetName());

      return;
   }

   bot->ApplyEnchantment(pItem, PERM_ENCHANTMENT_SLOT, false);
   pItem->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
   bot->ApplyEnchantment(pItem, PERM_ENCHANTMENT_SLOT, true);

   sLog.outDetail("%s: items was enchanted successfully!", bot->GetName());
}

uint32 PlayerbotAI::GetBuffedCount(Player* player, string spellname)
{
    Group* group = bot->GetGroup();
    uint32 bcount = 0;

    if (group)
    {
        for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->getSource();
            if (!member || !member->IsInWorld() && member->GetMapId() != bot->GetMapId())
                continue;

            if (!member->IsInGroup(player, true))
                continue;

            if (HasAura(spellname, member))
                bcount++;
        }
    }
    return bcount;
}

bool PlayerbotAI::CanMove()
{
    // do not allow if not vehicle driver
    if (IsInVehicle() && !IsInVehicle(true))
        return false;

    if (sServerFacade.IsFrozen(bot) || 
        sServerFacade.IsInRoots(bot) ||
        sServerFacade.IsFeared(bot) ||
        sServerFacade.IsCharmed(bot) ||
        bot->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION) ||
        bot->IsPolymorphed() ||
        bot->IsTaxiFlying() ||
        (sServerFacade.UnitIsDead(bot) && !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST)) ||
        bot->IsBeingTeleported() ||
        bot->hasUnitState(UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL))
        //bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLING))
        return false;

    MotionMaster& mm = *bot->GetMotionMaster();
#ifdef CMANGOS
    return mm.GetCurrentMovementGeneratorType() != TAXI_MOTION_TYPE;
#endif
#ifdef MANGOS
    return mm.GetCurrentMovementGeneratorType() != FLIGHT_MOTION_TYPE;
#endif
}

void PlayerbotAI::StopMoving()
{
    if (bot->IsTaxiFlying())
        return;

    if (IsInVehicle())
        return;

    // interrupt movement as much as we can...
    bot->InterruptMoving(true);
    MovementInfo mInfo = bot->m_movementInfo;
    float x, y, z;
    bot->GetPosition(x, y, z);
    float o = bot->GetPosition().o;
    mInfo.ChangePosition(x, y, z, o);
    WorldPacket data(MSG_MOVE_STOP);
#ifdef MANGOSBOT_TWO
    data << bot->GetObjectGuid().WriteAsPacked();
#endif
    data << mInfo;
    bot->GetSession()->HandleMovementOpcodes(data);

    bot->GetMotionMaster()->Clear(false, true);
    bot->GetMotionMaster()->MoveIdle();
}

bool PlayerbotAI::IsInRealGuild()
{
    if (!bot->GetGuildId())
        return false;

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    if (!guild)
        return false;

    uint32 leaderAccount = sObjectMgr.GetPlayerAccountIdByGUID(guild->GetLeaderGuid());
    if (!leaderAccount)
        return false;

    return !sPlayerbotAIConfig.IsInRandomAccountList(leaderAccount);
}

bool PlayerbotAI::HasPlayerRelation()
{
    if (HasRealPlayerMaster())
        return true;

    if (IsInRealGuild())
        return true;

    if (IsPlayerFriend())
        return true;

    if (!sRandomPlayerbotMgr.IsRandomBot(bot))
        return true;

    for (auto& p : sRandomPlayerbotMgr.GetPlayers())
        if (p.second && p.second->GetSocial()->HasFriend(bot->GetObjectGuid()))
        {
            SetPlayerFriend(true);
            return true;
        }

    return false;
}

void PlayerbotAI::QueueChatResponse(uint8 msgtype, ObjectGuid guid1, ObjectGuid guid2, std::string message, std::string chanName, std::string name)
{
    chatReplies.push(ChatQueuedReply(msgtype, guid1.GetCounter(), guid2.GetCounter(), message, chanName, name, time(0) + urand(inCombat ? 10 : 5, inCombat ? 25 : 15)));
}
