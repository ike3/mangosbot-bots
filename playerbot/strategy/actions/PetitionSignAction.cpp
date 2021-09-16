#include "botpch.h"
#include "../../playerbot.h"
#include "PetitionSignAction.h"
#include "../../RandomPlayerbotFactory.h"
#ifndef MANGOSBOT_ZERO
#ifdef CMANGOS
#include "Arena/ArenaTeam.h"
#endif
#ifdef MANGOS
#include "ArenaTeam.h"
#endif
#endif

using namespace std;
using namespace ai;

bool PetitionSignAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);
    ObjectGuid petitionGuid;
    ObjectGuid inviter;
    uint8 unk = 0;
    bool isArena = false;
    p >> petitionGuid >> inviter;
    uint32 type = 9;

#ifndef MANGOSBOT_ZERO
    QueryResult* result = CharacterDatabase.PQuery("SELECT `type` FROM `petition` WHERE `petitionguid` = '%u'", petitionGuid.GetCounter());
    if (!result)
    {
        return false;
    }

    Field* fields = result->Fetch();
    type = fields[0].GetUInt32();
    delete result;
#endif

    bool accept = true;

    if (type != 9)
    {
#ifndef MANGOSBOT_ZERO
        isArena = true;
        uint8 slot = ArenaTeam::GetSlotByType(ArenaType(type));
        if (bot->GetArenaTeamId(slot))
        {
            // player is already in an arena team
            ai->TellError("Sorry, I am already in such team");
            accept = false;
        }
#endif
    }
    else
    {
        if (bot->GetGuildId())
        {
            ai->TellError("Sorry, I am in a guild already");
            accept = false;
        }

        if (bot->GetGuildIdInvited())
        {
            ai->TellError("Sorry, I am invited to a guild already");
            accept = false;
        }

        QueryResult* result = CharacterDatabase.PQuery("SELECT playerguid FROM petition_sign WHERE player_account = '%u' AND petitionguid = '%u'", bot->GetSession()->GetAccountId(), petitionGuid.GetCounter());

        if (result)
        {
            ai->TellError("Sorry, I already signed this pettition");
            accept = false;
        }
    }

    Player* _inviter = sObjectMgr.GetPlayer(inviter);
    if (!inviter)
        return false;

    if (!accept || !ai->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_TALK, false, _inviter, true))
    {
        WorldPacket data(MSG_PETITION_DECLINE);
        data << petitionGuid;
        bot->GetSession()->HandlePetitionDeclineOpcode(data);
        sLog.outBasic("Bot #%d <%s> declines %s invite", bot->GetGUIDLow(), bot->GetName(), isArena ? "Arena" : "Guild");
        return false;
    }
    if (accept)
    {
        WorldPacket data(CMSG_PETITION_SIGN, 20);
        data << petitionGuid << unk;
        bot->GetSession()->HandlePetitionSignOpcode(data);
        bot->Say("Thanks for the invite!", LANG_UNIVERSAL);
        sLog.outBasic("Bot #%d <%s> accepts %s invite", bot->GetGUIDLow(), bot->GetName(), isArena ? "Arena" : "Guild");
        return true;
    }
    return false;
}

bool BuyPetitionAction::Execute(Event event)
{
    list<ObjectGuid> vendors = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest npcs")->Get();
    bool vendored = false, result = false;
    for (list<ObjectGuid>::iterator i = vendors.begin(); i != vendors.end(); ++i)
    {
        ObjectGuid vendorguid = *i;
        Creature* pCreature = bot->GetNPCIfCanInteractWith(vendorguid, UNIT_NPC_FLAG_PETITIONER);
        if (!pCreature)
            continue;

        string guildName = RandomPlayerbotFactory::CreateRandomGuildName();
        if (guildName.empty())
            continue;

        WorldPacket data(CMSG_PETITION_BUY, 8 + 4 + 4 + 4);

        data << pCreature->GetObjectGuid();
        data << uint32(0);
        data << uint32(0);
        data << guildName;
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint16(0);
        data << uint8(0);

        data << uint32(0); // index
        data << uint32(0);

        bot->GetSession()->HandlePetitionBuyOpcode(data);

        return true;
    }

    return false;
}

bool BuyPetitionAction::isUseful()
{
    return canBuyPetition(bot);
};

bool BuyPetitionAction::canBuyPetition(Player* bot)
{
    if (bot->GetGuildId())
        return false;

    if (bot->GetGuildIdInvited())
        return false;

    if (bot->GetItemByEntry(5863))
        return false;

    PlayerbotAI* ai = bot->GetPlayerbotAI();
    AiObjectContext* context = ai->GetAiObjectContext();

    if (ai->GetGuilderType() == GuilderType::SOLO)
        return false;

    if (ai->GetGrouperType() == GrouperType::SOLO)
        return false;

    if (!ai->HasStrategy("guild", BOT_STATE_NON_COMBAT))
        return false;

    uint32 cost = 1000; //GUILD_CHARTER_COST;

    if (AI_VALUE2(uint32, "free money for", uint32(NeedMoneyFor::guild)) < cost)
        return false;

    return true;
}

bool PetitionOfferAction::Execute(Event event)
{
    uint32 petitionEntry = 5863; //GUILD_CHARTER
    Item* petition = bot->GetItemByEntry(5863);

    if (!petition)
        return false;

    ObjectGuid guid = event.getObject();

    Player* master = GetMaster();
    if (!master)
    {
        if (!guid)
            guid = bot->GetSelectionGuid();
    }
    else {
        if (!guid)
            guid = master->GetSelectionGuid();
    }

    if (!guid)
        return false;

    Player* player = sObjectMgr.GetPlayer(guid);

    if (!player)
        return false;

    WorldPacket data(CMSG_OFFER_PETITION);

    data << petition->GetObjectGuid();
    data << guid;

    QueryResult* result = CharacterDatabase.PQuery("SELECT playerguid FROM petition_sign WHERE player_account = '%u' AND petitionguid = '%u'", player->GetSession()->GetAccountId(), petition->GetObjectGuid().GetCounter());

    if (result)
    {
        return false;
    }

    bot->GetSession()->HandleOfferPetitionOpcode(data);

    result = CharacterDatabase.PQuery("SELECT playerguid FROM petition_sign WHERE petitionguid = '%u'", petition->GetObjectGuid().GetCounter());
    uint8 signs = result ? (uint8)result->GetRowCount() : 0;

    context->GetValue<uint8>("petition signs")->Set(signs);

    return true;
};

bool PetitionOfferNearbyAction::Execute(Event event)
{
    uint32 found = 0;

    list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
    for (auto& i : nearGuids)
    {
        Player* player = sObjectMgr.GetPlayer(i);

        if (!player)
            continue;

        if (player->GetGuildId())
            continue;

        if (player->GetGuildIdInvited())
            continue;

        PlayerbotAI* botAi = player->GetPlayerbotAI();

        if (botAi)
        {
            /*
            if (botAi->GetGrouperType() == SOLO && !botAi->HasRealPlayerMaster()) //Do not invite solo players.
                continue;

            */
            if (botAi->HasActivePlayerMaster()) //Do not invite alts of active players. 
                continue;
        }
        else
        {
            if (!sPlayerbotAIConfig.randomBotGroupNearby)
                return false;
        }

        if (sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
            continue;

        //Parse rpg target to quest action.
        WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
        p << i;
        p.rpos(0);

        if (PetitionOfferAction::Execute(Event("petition offer nearby", p)))
            found++;
    }

    return found > 0;
};

bool PetitionTurnInAction::Execute(Event event)
{
    list<ObjectGuid> vendors = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest npcs")->Get();
    bool vendored = false, result = false;
    for (list<ObjectGuid>::iterator i = vendors.begin(); i != vendors.end(); ++i)
    {
        ObjectGuid vendorguid = *i;
        Creature* pCreature = bot->GetNPCIfCanInteractWith(vendorguid, UNIT_NPC_FLAG_PETITIONER);
        if (!pCreature)
            continue;

        WorldPacket data(CMSG_TURN_IN_PETITION, 8);

        Item* petition = bot->GetItemByEntry(5863);

        if (!petition)
            return false;

        data << petition->GetObjectGuid();

        bot->GetSession()->HandleTurnInPetitionOpcode(data);

        if (bot->GetGuildId())
        {
            Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
            uint32 st, cl, br, bc, bg;
            bg = urand(0, 51);
            bc = urand(0, 17);
            cl = urand(0, 17);
            br = urand(0, 7);
            st = urand(0, 180);
            guild->SetEmblem(st, cl, br, bc, bg);

            //LANG_GUILD_VETERAN -> can invite
            guild->SetRankRights(2, GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK | GR_RIGHT_INVITE);           
        }

        return true;
    }

    TravelTarget* oldTarget = context->GetValue<TravelTarget*>("travel target")->Get();

    //Select a new target to travel to. 
    TravelTarget newTarget = TravelTarget(ai);

    bool foundTarget = SetNpcFlagTarget(&newTarget, { UNIT_NPC_FLAG_PETITIONER });

    if (!foundTarget || !newTarget.isActive())
        return false;

    newTarget.setRadius(INTERACTION_DISTANCE);

    setNewTarget(&newTarget, oldTarget);

    return true;
};


bool BuyTabardAction::Execute(Event event)
{
    bool canBuy = ai->DoSpecificAction("buy", Event("buy tabard", "Hitem:5976:"));

    if (canBuy)
        return true;

    TravelTarget* oldTarget = context->GetValue<TravelTarget*>("travel target")->Get();

    //Select a new target to travel to. 
    TravelTarget newTarget = TravelTarget(ai);

    bool foundTarget = SetNpcFlagTarget(&newTarget, { UNIT_NPC_FLAG_TABARDDESIGNER }, "Tabard Vendor", { 5976 });

    if (!foundTarget || !newTarget.isActive())
        return false;

    newTarget.setRadius(INTERACTION_DISTANCE);

    setNewTarget(&newTarget, oldTarget);

    return true;
};