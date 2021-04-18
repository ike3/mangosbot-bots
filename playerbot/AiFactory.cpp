#include "../botpch.h"
#include "playerbot.h"
#include "AiFactory.h"
#include "strategy/Engine.h"

#include "strategy/priest/PriestAiObjectContext.h"
#include "strategy/mage/MageAiObjectContext.h"
#include "strategy/warlock/WarlockAiObjectContext.h"
#include "strategy/warrior/WarriorAiObjectContext.h"
#include "strategy/shaman/ShamanAiObjectContext.h"
#include "strategy/paladin/PaladinAiObjectContext.h"
#include "strategy/druid/DruidAiObjectContext.h"
#include "strategy/hunter/HunterAiObjectContext.h"
#include "strategy/rogue/RogueAiObjectContext.h"
#include "strategy/deathknight/DKAiObjectContext.h"
#include "Player.h"
#include "PlayerbotAIConfig.h"
#include "RandomPlayerbotMgr.h"
#include "BattleGroundMgr.h"


AiObjectContext* AiFactory::createAiObjectContext(Player* player, PlayerbotAI* ai)
{
    switch (player->getClass())
    {
    case CLASS_PRIEST:
        return new PriestAiObjectContext(ai);
        break;
    case CLASS_MAGE:
        return new MageAiObjectContext(ai);
        break;
    case CLASS_WARLOCK:
        return new WarlockAiObjectContext(ai);
        break;
    case CLASS_WARRIOR:
        return new WarriorAiObjectContext(ai);
        break;
    case CLASS_SHAMAN:
        return new ShamanAiObjectContext(ai);
        break;
    case CLASS_PALADIN:
        return new PaladinAiObjectContext(ai);
        break;
    case CLASS_DRUID:
        return new DruidAiObjectContext(ai);
        break;
    case CLASS_HUNTER:
        return new HunterAiObjectContext(ai);
        break;
    case CLASS_ROGUE:
        return new RogueAiObjectContext(ai);
        break;
#ifdef MANGOSBOT_TWO
    case CLASS_DEATH_KNIGHT:
        return new DKAiObjectContext(ai);
        break;
#endif
    }
    return new AiObjectContext(ai);
}

int AiFactory::GetPlayerSpecTab(Player* bot)
{
    if (bot->getLevel() >= 10)
    {
        map<uint32, int32> tabs = GetPlayerSpecTabs(bot);

        int tab = -1, max = 0;
        for (uint32 i = 0; i < uint32(3); i++)
        {
            if (tab == -1 || max < tabs[i])
            {
                tab = i;
                max = tabs[i];
            }
        }
        return tab;
    }
    else
    {
        int tab = 0;

        switch (bot->getClass())
        {
        case CLASS_MAGE:
            tab = 1;
            break;
        case CLASS_PALADIN:
            tab = 2;
            break;
        case CLASS_PRIEST:
            tab = 1;
            break;
        }
        return tab;
    }
}

map<uint32, int32> AiFactory::GetPlayerSpecTabs(Player* bot)
{
    map<uint32, int32> tabs;
    for (uint32 i = 0; i < uint32(3); i++)
        tabs[i] = 0;

    uint32 classMask = bot->getClassMask();
    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if ((classMask & talentTabInfo->ClassMask) == 0)
            continue;

        int maxRank = 0;
        for (int rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (!talentInfo->RankID[rank])
                continue;

            uint32 spellid = talentInfo->RankID[rank];
            if (spellid && bot->HasSpell(spellid))
                maxRank = rank + 1;

        }
        tabs[talentTabInfo->tabpage] += maxRank;
    }

    return tabs;
}

BotRoles AiFactory::GetPlayerRoles(Player* player)
{
    BotRoles role = BOT_ROLE_NONE;
    int tab = GetPlayerSpecTab(player);
    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
                role = BOT_ROLE_DPS;
            else
                role = BOT_ROLE_HEALER;
            break;
        case CLASS_SHAMAN:
            if (tab == 2)
                role = BOT_ROLE_HEALER;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                role = BOT_ROLE_TANK;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_PALADIN:
            if (tab == 0)
                role = BOT_ROLE_HEALER;
            else if (tab == 1)
                role = BOT_ROLE_TANK;
            else if (tab == 2)
                role = BOT_ROLE_DPS;
            break;
        case CLASS_DRUID:
            if (tab == 0)
                role = BOT_ROLE_DPS;
            else if (tab == 1)
                role = (BotRoles)(BOT_ROLE_TANK | BOT_ROLE_DPS);
            else if (tab == 2)
                role = BOT_ROLE_HEALER;
            break;
        default:
            role = BOT_ROLE_DPS;
            break;
    }
    return role;
}

void AiFactory::AddDefaultCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* engine)
{
    int tab = GetPlayerSpecTab(player);

    if (!player->InBattleGround())
    {
        engine->addStrategies("racials", "chat", "default", "aoe", "potions", "cast time", "conserve mana", "duel", NULL);
    }

    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
            {
                engine->addStrategies("dps", "threat", NULL);
                if (player->getLevel() >= 4)
                    engine->addStrategy("dps debuff");
            }
            else if (tab == 0)
            {
                engine->addStrategies("holy", NULL);
                    //if (player->getLevel() >= 4)
                       //engine->addStrategy("dps debuff");
            }
            else
                engine->addStrategy("heal");

            engine->addStrategies("dps assist", "flee", "cure", "ranged", "cc", NULL);
            break;
        case CLASS_MAGE:
            if (tab == 0)
                engine->addStrategies("arcane", "threat", NULL);
            else if (tab == 1)
                engine->addStrategies("fire", "fire aoe", "threat", NULL);
            else
                engine->addStrategies("frost", "frost aoe", "threat", "dps aoe", NULL);

            engine->addStrategies("dps", "dps assist", "flee", "cure", "ranged", "cc", NULL);
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                engine->addStrategies("tank", "tank aoe", "close", NULL);
            else
                engine->addStrategies("dps", "dps assist", "threat", "close", NULL);
            break;
        case CLASS_SHAMAN:
            if (tab == 0)
                engine->addStrategies("caster", "caster aoe", "bmana", "threat", "flee", "ranged", NULL);
            else if (tab == 2)
                engine->addStrategies("heal", "bmana", "flee", "ranged", NULL);
            else
                engine->addStrategies("dps", "melee aoe", "bdps", "threat", "close", NULL);

            engine->addStrategies("dps assist", "cure", "totems", NULL);
            break;
        case CLASS_PALADIN:
            if (tab == 1)
                engine->addStrategies("tank", "tank aoe", "bthreat", "cure", "barmor", "bstats", "close", "cc", NULL);
			else if(tab == 0)
                engine->addStrategies("heal", "bmana", "cure", "flee", "barmor", "ranged", NULL);
            else
                engine->addStrategies("dps", "bdps", "dps assist", "cure", "baoe", "close", "cc", NULL);

            if (player->getLevel() < 14)
            {
                engine->addStrategy("bdps");
            }
            if (player->getLevel() < 16)
            {
                engine->addStrategy("barmor");
            }
            break;
        case CLASS_DRUID:
            if (tab == 0)
            {
                engine->addStrategies("caster", "cure", "caster aoe", "threat", "flee", "dps assist", "ranged", "cc", NULL);
                if (player->getLevel() > 19)
                    engine->addStrategy("caster debuff");
            }
            else if (tab == 2)
                engine->addStrategies("heal", "cure", "flee", "dps assist", "ranged", "cc", NULL);
            else
            {
                engine->removeStrategy("ranged");
                engine->addStrategies("bear", "tank aoe", "flee", "close", NULL);
            }
            break;
        case CLASS_HUNTER:
            engine->addStrategies("dps", "bdps", "threat", "dps assist", "ranged", "pet", "cc", NULL);
            if (player->getLevel() > 19)
                engine->addStrategy("dps debuff");
            break;
        case CLASS_ROGUE:
            engine->addStrategies("dps", "threat", "dps assist", "aoe", "close", "cc", "behind", NULL);
            break;
        case CLASS_WARLOCK:
            if (tab == 1)
                engine->addStrategies("tank", "threat", NULL);
            else
                engine->addStrategies("dps", "threat", NULL);

            if (player->getLevel() > 19)
                engine->addStrategy("dps debuff");

            engine->addStrategies("dps assist", "flee", "ranged", "cc", "pet", NULL);
            break;
#ifdef MANGOSBOT_TWO
        case CLASS_DEATH_KNIGHT:
            if (tab == 0)
                engine->addStrategies("blood", NULL);
            else if (tab == 1)
                engine->addStrategies("frost", "frost aoe", "dps assist", "threat", NULL);
            else
                engine->addStrategies("unholy", "unholy aoe", "dps assist", "threat", NULL);
            break;
#endif
    }

	if (sRandomPlayerbotMgr.IsRandomBot(player))
	{
        if (!player->GetGroup())
        {
            engine->ChangeStrategy(sPlayerbotAIConfig.randomBotCombatStrategies);

            engine->addStrategy("flee");
            engine->addStrategy("boost");

            if (player->getClass() == CLASS_WARLOCK)
            {
                engine->removeStrategy("ranged");
            }
            
            if (player->getClass() == CLASS_DRUID && tab == 2)
            {
                engine->addStrategies("caster", "caster aoe", NULL);
            }

            if (player->getClass() == CLASS_DRUID && tab == 1 && urand(0, 100) > 50 && player->getLevel() > 19)
            {
                engine->addStrategy("dps");
            }

            if (player->getClass() == CLASS_PRIEST && tab == 1)
            {
                engine->removeStrategy("heal");
                engine->addStrategies("shadow aoe", "holy", NULL);
            }

            if (player->getClass() == CLASS_SHAMAN && tab == 2)
            {
                engine->addStrategies("caster", "caster aoe", NULL);
            }

            if (player->getClass() == CLASS_PALADIN && tab == 0)
            {
                engine->addStrategies("dps", "close", NULL);
            }

            if (player->getClass() == CLASS_ROGUE)
                engine->addStrategies("stealth", NULL);

            if (player->getClass() != CLASS_HUNTER)
                engine->removeStrategy("ranged");
        }

        if (player->getClass() == CLASS_ROGUE)
            engine->addStrategy("stealth");
    }
    else
    {
        engine->ChangeStrategy(sPlayerbotAIConfig.combatStrategies);
    }

    // Battleground switch
    if (player->InBattleGround())
    {
        if (player->GetBattleGroundTypeId() == BATTLEGROUND_WS)
            engine->addStrategy("warsong");

#ifndef MANGOSBOT_ZERO
        if (player->InArena())
        {
            engine->addStrategy("arena");
            engine->removeStrategy("ranged");
        }
#endif
        engine->addStrategies("racials", "chat", "default", "aoe", "potions", "conserve mana", "cast time", NULL);
        engine->removeStrategy("custom::say");
        engine->removeStrategy("flee");
        engine->removeStrategy("threat");
        engine->addStrategy("boost");

        if ((player->getClass() == CLASS_DRUID && tab == 2) || (player->getClass() == CLASS_SHAMAN && tab == 2))
            engine->addStrategies("caster", "caster aoe", NULL);

        if (player->getClass() != CLASS_HUNTER)
            engine->removeStrategy("ranged");

        if (player->getClass() == CLASS_DRUID && tab == 1 && engine->HasStrategy("dps"))
            engine->addStrategy("behind");
        
        if (player->getClass() == CLASS_ROGUE)
            engine->addStrategy("stealth");
    }
}

Engine* AiFactory::createCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext) {
	Engine* engine = new Engine(facade, AiObjectContext);
    AddDefaultCombatStrategies(player, facade, engine);
    return engine;
}

void AiFactory::AddDefaultNonCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* nonCombatEngine)
{
    int tab = GetPlayerSpecTab(player);

    switch (player->getClass()){
        case CLASS_PRIEST:
            nonCombatEngine->addStrategies("dps assist", "cure", NULL);
            break;
        case CLASS_PALADIN:
            if (tab == 1)
                nonCombatEngine->addStrategies("bthreat", "tank aoe", "barmor", "bstats", NULL);
            else if (tab == 0)
                nonCombatEngine->addStrategies("dps assist", "barmor", "bmana", NULL);
            else
                nonCombatEngine->addStrategies("dps assist", "baoe", "bdps", NULL);

            if (player->getLevel() < 14)
                nonCombatEngine->addStrategies("bdps", NULL);
            if (player->getLevel() < 16)
                nonCombatEngine->addStrategies("barmor", NULL);

            nonCombatEngine->addStrategies("cure", NULL);
            break;
        case CLASS_HUNTER:
            nonCombatEngine->addStrategies("bdps", "dps assist", NULL);
            break;
        case CLASS_SHAMAN:
            if (tab == 0 || tab == 2)
                nonCombatEngine->addStrategy("bmana");
            else
                nonCombatEngine->addStrategy("bdps");

            nonCombatEngine->addStrategies("dps assist", "cure", NULL);
            break;
        case CLASS_MAGE:
            if (tab == 1)
                nonCombatEngine->addStrategy("bdps");
            else
                nonCombatEngine->addStrategy("bmana");

            nonCombatEngine->addStrategies("dps assist", "cure", NULL);
            break;
        case CLASS_DRUID:
            if (tab == 1)
                nonCombatEngine->addStrategy("tank aoe");
            else
                nonCombatEngine->addStrategies("dps assist", "cure", NULL);
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                nonCombatEngine->addStrategy("tank aoe");
            else
                nonCombatEngine->addStrategy("dps assist");
            break;
        case CLASS_WARLOCK:
            nonCombatEngine->addStrategies("pet", "dps assist", NULL);
            break;
#ifdef MANGOSBOT_TWO
        case CLASS_DEATH_KNIGHT:
            if (tab == 0)
                nonCombatEngine->addStrategy("tank aoe");
            else
                nonCombatEngine->addStrategy("dps assist");
            break;
#endif
        default:
            nonCombatEngine->addStrategy("dps assist");
            break;
    }

    if (!player->InBattleGround())
    {
        nonCombatEngine->addStrategies("nc", "food", "stay", "sit", "chat", "follow",
            "default", "quest", "loot", "gather", "duel", "emote", "conserve mana", "buff", "mount", NULL);
    }

    if (sRandomPlayerbotMgr.IsRandomBot(player))
    {   
        if (!player->GetGroup() || player->GetGroup()->GetLeaderGuid() == player->GetObjectGuid())
        {
            // let 50% of random not grouped (or grp leader) bots help other players
            if (!urand(0, 4))
                nonCombatEngine->addStrategy("attack tagged");

            nonCombatEngine->addStrategy("pvp");
            nonCombatEngine->addStrategy("collision");
            if (sPlayerbotAIConfig.autoDoQuests)
                nonCombatEngine->addStrategy("travel");
            if (sPlayerbotAIConfig.randomBotJoinLfg)
                nonCombatEngine->addStrategy("lfg");
            nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig.randomBotNonCombatStrategies);
        }
        else {
            PlayerbotAI* botAi = player->GetPlayerbotAI();
            if (botAi)
            {
                Player* master = botAi->GetMaster();
                if (master)
                {
                    if (master->GetPlayerbotAI())
                    {
                        nonCombatEngine->addStrategy("pvp");
                        nonCombatEngine->addStrategy("collision");
                        if (sPlayerbotAIConfig.autoDoQuests)
                            nonCombatEngine->addStrategy("travel");
                        nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig.randomBotNonCombatStrategies);
                    }
                }
            }
        }
    }
    else
    {
        nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig.nonCombatStrategies);
    }

    // Battleground switch
    if (player->InBattleGround())
    {
        nonCombatEngine->addStrategies("nc", "chat", "dps assist",
            "default", "emote", "buff", "food", "conserve mana", "collision", "mount", NULL);
        nonCombatEngine->removeStrategy("custom::say");
        nonCombatEngine->removeStrategy("travel");
        nonCombatEngine->removeStrategy("rpg");
        nonCombatEngine->removeStrategy("grind");

        if (player->GetBattleGroundTypeId() == BATTLEGROUND_WS)
            nonCombatEngine->addStrategy("warsong");

#ifndef MANGOSBOT_ZERO
        if (player->InArena())
        {
            nonCombatEngine->addStrategy("arena");
            nonCombatEngine->removeStrategy("mount");
        }
#endif
    }
}

Engine* AiFactory::createNonCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext) {
	Engine* nonCombatEngine = new Engine(facade, AiObjectContext);

    AddDefaultNonCombatStrategies(player, facade, nonCombatEngine);
	return nonCombatEngine;
}

void AiFactory::AddDefaultDeadStrategies(Player* player, PlayerbotAI* const facade, Engine* deadEngine)
{
    deadEngine->addStrategies("dead", "stay", "chat", "default", "follow", NULL);
    if (sRandomPlayerbotMgr.IsRandomBot(player) && !player->GetGroup())
    {
        deadEngine->removeStrategy("follow");
    }

    if (player->InBattleGround() && player->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
    {
        deadEngine->addStrategies("warsong", NULL);
    }
}

Engine* AiFactory::createDeadEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext) {
    Engine* deadEngine = new Engine(facade, AiObjectContext);
    AddDefaultDeadStrategies(player, facade, deadEngine);
    return deadEngine;
}
