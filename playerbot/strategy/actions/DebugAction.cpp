#include "botpch.h"
#include "../../playerbot.h"
#include "DebugAction.h"
#include "../../PlayerbotAIConfig.h"
#include <playerbot/TravelNode.h>
#include "ChooseTravelTargetAction.h"
#include "strategy/values/SharedValueContext.h"
#include "../../LootObjectStack.h"
#include "../../../game/GameEvents/GameEventMgr.h"

using namespace ai;

bool DebugAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        master = bot;

    string text = event.getParam();
    if (text == "scan")
    {
        sPlayerbotAIConfig.openLog("scan.csv", "w");

        uint32 i;
        for (auto p : WorldPosition().getCreaturesNear())
        {
            WorldPosition pos(p);

            const uint32 zoneId = sTerrainMgr.GetZoneId(pos.getMapId(), pos.getX(), pos.getY(), pos.getZ());
            const uint32 areaId = sTerrainMgr.GetAreaId(pos.getMapId(), pos.getX(), pos.getY(), pos.getZ());

            ostringstream out;
            out << zoneId << "," << areaId << "," << pos.getAreaFlag() << "," << (pos.getAreaName().empty() ? "none" : pos.getAreaName()) << ",";

            pos.printWKT(out);

            sPlayerbotAIConfig.log("scan.csv", out.str().c_str());

            if (zoneId == 0 && areaId == 0)
            {
                const uint32 zoneId = sTerrainMgr.GetZoneId(pos.getMapId(), pos.getX(), pos.getY(), pos.getZ());
                const uint32 areaId = sTerrainMgr.GetAreaId(pos.getMapId(), pos.getX(), pos.getY(), pos.getZ());
                sPlayerbotAIConfig.log("x", out.str().c_str());
            }
            else
            {
                const uint32 zoneId = sTerrainMgr.GetZoneId(pos.getMapId(), pos.getX(), pos.getY(), pos.getZ());
                const uint32 areaId = sTerrainMgr.GetAreaId(pos.getMapId(), pos.getX(), pos.getY(), pos.getZ());
                sPlayerbotAIConfig.log("y", out.str().c_str());
            }

            i = zoneId;
        }
        return i == 0;
    }
    else if (text.find("test") != std::string::npos)
    {
        auto selection = bot->GetSelectionGuid();

        if (!selection)
            return false;

        Unit* unit = ai->GetUnit(bot->GetSelectionGuid());

        if (!unit)
            return false;

        sServerFacade.SetFacingTo(bot, unit);
        sServerFacade.SetFacingTo(unit, bot);

        return true;
    }
    else if (text.find("do ") != std::string::npos)
    {
        return ai->DoSpecificAction(text.substr(3), Event(), true);
    }
    else if (text.find("poi ") != std::string::npos)
    {        
        WorldPosition botPos = WorldPosition(bot);

        WorldPosition poiPoint = botPos;
        string name = "bot";

        vector<string> args = Qualified::getMultiQualifiers(text.substr(4));
        TravelDestination* dest = ChooseTravelTargetAction::FindDestination(bot, args[0]);

        if (dest)
        {
            vector <WorldPosition*> points = dest->nextPoint(&botPos, true);

            if (!points.empty())
            {
                poiPoint = *points.front();
                name = dest->getTitle();
            }
        }

        if (args.size() == 1)
            args.push_back("99");
        if (args.size() == 2)
            args.push_back("7");
        if (args.size() == 3)
            args.push_back("0");

        ai->Poi(poiPoint.coord_x, poiPoint.coord_y, name, nullptr, stoi(args[1]), stoi(args[2]), stoi(args[3]));

        return true;
    }
    else if (text.find("printmap") != std::string::npos)
    {
        sTravelNodeMap.printMap();
        sTravelNodeMap.printNodeStore();
        return true;
    }
    else if (text.find("npc") != std::string::npos)
    {
        ostringstream out;

        GuidPosition guidP = ai->GetMaster()->GetSelectionGuid();

        if (text.size() > 4)
        {
            string link = text.substr(4);

            if (!link.empty())
            {
                list<int32> entries = chat->parseWorldEntries(link);
                if (!entries.empty())
                {

                    int32 entry = entries.front();

                    for (auto cre : WorldPosition(bot).getCreaturesNear(0.0f, entry))
                    {
                        guidP = GuidPosition(cre);
                        break;
                    }
                }
            }
        }

        if (!guidP)
            return false;

        if (guidP.GetWorldObject())
            out << chat->formatWorldobject(guidP.GetWorldObject());
        
        out << " (e:" << guidP.GetEntry();
        
        if (guidP.GetUnit())
            out << ",level:" << guidP.GetUnit()->GetLevel();
            
        out << ") ";

        guidP.printWKT(out);

        out << "[a:" << guidP.getArea()->area_name[0]; 

        if (guidP.getArea() && guidP.getAreaLevel())
            out << " level: " << guidP.getAreaLevel();
        if (guidP.getArea()->zone && GetAreaEntryByAreaID(guidP.getArea()->zone))
        {
            out << " z:" << GetAreaEntryByAreaID(guidP.getArea()->zone)->area_name[0];
            if (sTravelMgr.getAreaLevel(guidP.getArea()->zone))
                out << " level: " << sTravelMgr.getAreaLevel(guidP.getArea()->zone);
        }

        out << "] ";

        uint16 event_id = sGameEventMgr.GetGameEventId<Creature>(guidP.GetCounter());

        if (event_id)
            out << " event:" << event_id << (sGameEventMgr.IsActiveEvent(event_id) ? " active" : " inactive");

        uint16 topPoolId = sPoolMgr.IsPartOfTopPool<Creature>(guidP.GetCounter());

        if (topPoolId)
            out << " pool:" << topPoolId << (sGameEventMgr.GetGameEventId<Pool>(topPoolId) ? " event" : " nonevent");

        ai->TellMasterNoFacing(out);


        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_GOSSIP))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_GOSSIP");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_QUESTGIVER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_QUESTGIVER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_VENDOR))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_VENDOR");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_FLIGHTMASTER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_FLIGHTMASTER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_TRAINER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_TRAINER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_SPIRITHEALER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_SPIRITHEALER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_SPIRITGUIDE))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_SPIRITGUIDE");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_INNKEEPER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_INNKEEPER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_BANKER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_BANKER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_PETITIONER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_PETITIONER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_TABARDDESIGNER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_TABARDDESIGNER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_BATTLEMASTER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_BATTLEMASTER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_AUCTIONEER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_AUCTIONEER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_STABLEMASTER))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_STABLEMASTER");
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_REPAIR))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_REPAIR");
#ifdef MANGOSBOT_ZERO
        if (guidP.HasNpcFlag(UNIT_NPC_FLAG_OUTDOORPVP))
            ai->TellMasterNoFacing("UNIT_NPC_FLAG_OUTDOORPVP");
#endif

        unordered_map<ReputationRank, string> reaction;

        reaction[REP_HATED] = "REP_HATED";
        reaction[REP_HOSTILE] = "REP_HOSTILE";
        reaction[REP_UNFRIENDLY] = "REP_UNFRIENDLY";
        reaction[REP_NEUTRAL] = "REP_NEUTRAL";
        reaction[REP_FRIENDLY] = "REP_FRIENDLY";
        reaction[REP_HONORED] = "REP_HONORED";
        reaction[REP_REVERED] = "REP_REVERED";
        reaction[REP_EXALTED] = "REP_EXALTED";
        
        if (guidP.GetUnit())
        {
            ostringstream out;
            out << "unit to bot:" << reaction[guidP.GetUnit()->GetReactionTo(bot)];

            Unit* ubot = bot;
            out << " bot to unit:" << reaction[ubot->GetReactionTo(guidP.GetUnit())];

            out << " npc to bot:" << reaction[guidP.GetReactionTo(bot)];
            out << " bot to npc:" << reaction[GuidPosition(bot).GetReactionTo(guidP)];

            if (GuidPosition(HIGHGUID_UNIT, guidP.GetEntry()).IsHostileTo(bot))
                out << "[hostile]";
            if (GuidPosition(HIGHGUID_UNIT, guidP.GetEntry()).IsFriendlyTo(bot))
                out << "[friendly]";

            ai->TellMasterNoFacing(out);
        }

        return true;
    }
    else if (text.find("travel ") != std::string::npos)
    {
        WorldPosition botPos = WorldPosition(bot);

        string destination = text.substr(7);

        TravelDestination* dest = ChooseTravelTargetAction::FindDestination(bot, destination);
        if (dest)
        {
            vector <WorldPosition*> points = dest->nextPoint(&botPos, true);

            if (points.empty())
                return false;

            vector<WorldPosition> beginPath, endPath;
            TravelNodeRoute route = sTravelNodeMap.getRoute(botPos, *points.front(), beginPath, bot);

            ostringstream out; out << "Traveling to " << dest->getTitle() << ": ";

            for (auto node : route.getNodes())
            {
                out << node->getName() << ", ";
            }

            ai->TellMasterNoFacing(out.str());

            return true;
        }
        else
        {
            ai->TellMasterNoFacing("Destination " + destination + " not found.");
            return true;
        }
    }
    else if (text.find("quest ") != std::string::npos)
    {
        WorldPosition botPos(bot);
        uint32 questId = stoi(text.substr(6));

        Quest const* quest = sObjectMgr.GetQuestTemplate(questId);

        if (!quest || sTravelMgr.quests.find(questId) == sTravelMgr.quests.end())
        {
            ai->TellMasterNoFacing("Quest " + text.substr(6) + " not found.");
            return false;
        }

        ostringstream out;

        out << quest->GetTitle() << ": ";

        ai->TellMasterNoFacing(out);

        QuestContainer* cont = sTravelMgr.quests[questId];

        uint32 i = 0;

        vector<QuestTravelDestination*> dests = cont->questGivers;

        std::sort(dests.begin(), dests.end(), [botPos](QuestTravelDestination* i, QuestTravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });


        for (auto g : dests)
        {
            ostringstream out;

            if (g->isActive(bot))
                out << "(ACTIVE)";

            out << g->getTitle().c_str();

            out << " (" << g->distanceTo(botPos) << "y)";

            ai->TellMasterNoFacing(out);

            if (i >= 10)
                break;

            i++;
        }

        for (uint32 o = 0; o < 4; o++)
        {
            i = 0;

            dests.clear();

            for (auto g : cont->questObjectives)
            {
                QuestObjectiveTravelDestination* d = (QuestObjectiveTravelDestination*)g;
                if (d->getObjective() == o)
                    dests.push_back(g);
            }

            std::sort(dests.begin(), dests.end(), [botPos](QuestTravelDestination* i, QuestTravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });

            for (auto g : dests)
            {
                ostringstream out;

                if (g->isActive(bot))
                    out << "(ACTIVE)";

                out << g->getTitle().c_str();

                QuestObjectiveTravelDestination* d = (QuestObjectiveTravelDestination*)g;

                if (d->getEntry())
                    out << "[" << ObjectMgr::GetCreatureTemplate(d->getEntry())->MaxLevel << "]";

                out << " (" << g->distanceTo(botPos) << "y)";

                ai->TellMasterNoFacing(out);

                if (i >= 5)
                    break;

                i++;
            }
        }

        i = 0;

       dests = cont->questGivers;

        std::sort(dests.begin(), dests.end(), [botPos](QuestTravelDestination* i, QuestTravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });


        for (auto g : dests)
        {
            ostringstream out;

            if (g->isActive(bot))
                out << "(ACTIVE)";

            out << g->getTitle().c_str();

            out << " (" << g->distanceTo(botPos) << "y)";

            ai->TellMasterNoFacing(out);

            if (i >= 10)
                break;

            i++;
        }

        return true;
    }
    else if (text.find("quest") != std::string::npos)
    {
        ostringstream out;
        out << sTravelMgr.quests.size() << " quests ";

        uint32 noT = 0, noG = 0, noO = 0;

        for (auto q : sTravelMgr.quests)
        {
            if (q.second->questGivers.empty())
                noG++;

            if (q.second->questTakers.empty())
                noT++;

            if (q.second->questObjectives.empty())
                noO++;
        }

        out << noG << "|" << noT << "|" << noO << " bad.";

        ai->TellMasterNoFacing(out);

        return true;
    }
    else if (text.find("bquest") != std::string::npos)
    {
        ostringstream out;
        out << "bad quests:";

        uint32 noT = 0, noG = 0, noO = 0;

        for (auto q : sTravelMgr.quests)
        {
            Quest const* quest = sObjectMgr.GetQuestTemplate(q.first);

            if (!quest)
            {
                out << " " << q.first << " does not exists";
                continue;
            }

            if (q.second->questGivers.empty() || q.second->questTakers.empty() || q.second->questObjectives.empty())
                out << quest->GetTitle() << " ";

            if (q.second->questGivers.empty())
                out << " no G";

            if (q.second->questTakers.empty())
                out << " no T";

            if (q.second->questObjectives.empty())
                out << " no O";
        }
        ai->TellMasterNoFacing(out);

    }
    else if (text.find("values ") != std::string::npos)
    {
        ai->TellMasterNoFacing(ai->GetAiObjectContext()->FormatValues(text.substr(7)));

        return true;
    }
    else if (text.find("loot ") != std::string::npos)
    {
        std::string textSubstr;
        std::ostringstream out;
        for (auto itemId : chat->parseItems(textSubstr = text.substr(5)))
        {
            list<int32> entries = GAI_VALUE2(list<int32>, "item drop list", itemId);

            if (entries.empty())
                out << chat->formatItem(sObjectMgr.GetItemPrototype(itemId), 0, 0) << " no sources found.";
            else
                out << chat->formatItem(sObjectMgr.GetItemPrototype(itemId), 0, 0) << " " << to_string(entries.size()) << " sources found:";

            ai->TellMasterNoFacing(out);
            out.str("");
            out.clear();

            vector<pair<int32, float>> chances;

            for (auto entry : entries)
            {
                std::vector<std::string> qualifiers = { to_string(entry), to_string(itemId) };
                std::string qualifier = Qualified::MultiQualify(qualifiers);
                float chance = GAI_VALUE2(float, "loot chance", qualifier);
                if(chance > 0)
                    chances.push_back(make_pair(entry, chance));
            }

            std::sort(chances.begin(), chances.end(), [](std::pair<int32, float> i, std::pair<int32, float> j) {return i.second > j.second; });

            chances.resize(std::min(20, (int)chances.size()));

            for (auto chance : chances)
            {
                out << chat->formatWorldEntry(chance.first) << ": " << chance.second << "%";
                ai->TellMasterNoFacing(out);
                out.str("");
                out.clear();
            }
        }
        return true;
    }
    else if (text.find("loot") != std::string::npos)
    {
    bool doAction = ai->DoSpecificAction("add all loot", Event(), true);

    if (doAction)
        ai->TellMasterNoFacing("Added new loot");
    else
        ai->TellMasterNoFacing("No new loot");

    LootObjectStack* loots = AI_VALUE(LootObjectStack*, "available loot");

    for (int i = 0; i < 2000; i++)
    {
        LootObject loot = loots->GetLoot();

        if (loot.IsEmpty())
            break;

        WorldObject* wo = ai->GetWorldObject(loot.guid);

        if (wo)
            ai->TellMasterNoFacing(chat->formatWorldobject(wo) + " " + (loot.IsLootPossible(bot) ? "can loot" : "can not loot"));
        else
            ai->TellMasterNoFacing(to_string(loot.guid) + " " + (loot.IsLootPossible(bot) ? "can loot" : "can not loot") + " " + to_string(loot.guid.GetEntry()));

        if (loot.guid.IsGameObject())
        {
            GameObject* go = ai->GetGameObject(loot.guid);

            if (go->ActivateToQuest(bot))
                ai->TellMasterNoFacing(to_string(go->GetGoType()) + " for quest");
            else
                ai->TellMasterNoFacing(to_string(go->GetGoType()));
        }

        loots->Remove(loot.guid);
    }

    ai->DoSpecificAction("add all loot", Event(), true);

    return true;
    }
    else if (text.find("drops ") != std::string::npos)
    {
    std::string textSubstr;
    std::ostringstream out;
    for (auto entry : chat->parseWorldEntries(textSubstr = text.substr(6)))
    {
        list<uint32> itemIds = GAI_VALUE2(list<uint32>, "entry loot list", entry);

        if (itemIds.empty())
            out << chat->formatWorldEntry(entry) << " no drops found.";
        else
            out << chat->formatWorldEntry(entry) << " " << to_string(itemIds.size()) << " drops found:";

        ai->TellMasterNoFacing(out);
        out.str("");
        out.clear();

        vector<pair<uint32, float>> chances;

        for (auto itemId : itemIds)
        {
            std::vector<std::string> qualifiers = { to_string(entry) , to_string(itemId) };
            std::string qualifier = Qualified::MultiQualify(qualifiers);
            float chance = GAI_VALUE2(float, "loot chance", qualifier);
            if (chance > 0 && sObjectMgr.GetItemPrototype(itemId))
            {
                chances.push_back(make_pair(itemId, chance));
            }
        }

        std::sort(chances.begin(), chances.end(), [](std::pair<uint32, float> i, std::pair<int32, float> j) {return i.second > j.second; });

        chances.resize(std::min(20, (int)chances.size()));

        for (auto chance : chances)
        {
            out << chat->formatItem(sObjectMgr.GetItemPrototype(chance.first), 0, 0) << ": " << chance.second << "%";
            ai->TellMasterNoFacing(out);
            out.str("");
            out.clear();
        }
    }
    return true;
    }
    else if (text.find("taxi") != std::string::npos)
    {
        for (uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
        {
            if (!bot->m_taxi.IsTaximaskNodeKnown(i))
                continue;

            TaxiNodesEntry const* taxiNode = sTaxiNodesStore.LookupEntry(i);

            ostringstream out;

            out << taxiNode->name[0];

            ai->TellMasterNoFacing(out);
        }
    return true;
    }
    else if (text.find("add node") != std::string::npos)
    {
        WorldPosition pos(bot);

        string name = "USER:" + text.substr(9);

        TravelNode* startNode = sTravelNodeMap.addNode(pos, name, false, false);

        for (auto& endNode : sTravelNodeMap.getNodes(pos, 2000))
        {
            endNode->setLinked(false);
        }

        ai->TellMasterNoFacing("Node " + name + " created.");
        
        sTravelNodeMap.setHasToGen();

        return true;
    }
    else if (text.find("rem node") != std::string::npos)
    {
        WorldPosition pos(bot);

        TravelNode* startNode = sTravelNodeMap.getNode(pos, nullptr, 50);

        if (!startNode)
            return false;

        if (startNode->isImportant())
        {
            ai->TellMasterNoFacing("Node can not be removed.");
        }
        sTravelNodeMap.m_nMapMtx.lock();
        sTravelNodeMap.removeNode(startNode);
        ai->TellMasterNoFacing("Node removed.");
        sTravelNodeMap.m_nMapMtx.unlock();

        sTravelNodeMap.setHasToGen();

        return true;
    }
    else if (text.find("reset node") != std::string::npos) {
        for (auto& node : sTravelNodeMap.getNodes())
            node->setLinked(false);
        return true;
    }
    else if (text.find("reset path") != std::string::npos) {
        for (auto& node : sTravelNodeMap.getNodes())
            for (auto& path : *node->getLinks())
                node->removeLinkTo(path.first, true);
        return true;
    }
    else if (text.find("gen node") != std::string::npos) {

        //Pathfinder
        sTravelNodeMap.generateNodes();
        return true;
    }
    else if (text.find("gen path") != std::string::npos) {
    sTravelNodeMap.generatePaths();
    return true;
    }
    else if (text.find("crop path") != std::string::npos) {
        sTravelNodeMap.removeUselessPaths();
        return true;
    }
    else if (text.find("save node") != std::string::npos)
    {
        sTravelNodeMap.printNodeStore();
        sTravelNodeMap.saveNodeStore();
        return true;
    }
    else if (text.find("load node") != std::string::npos)
    {
        std::thread t([] {if (sTravelNodeMap.removeNodes())
            sTravelNodeMap.loadNodeStore(); });

        t.detach();

        return true;
    }
    else if (text.find("show node") != std::string::npos)
    {
        WorldPosition pos(bot);

        vector<TravelNode*> nodes = sTravelNodeMap.getNodes(pos, 500);

        for (auto& node : nodes)
        {
            for (auto& l : *node->getLinks())
            {
                Unit* start = nullptr;
                list<ObjectGuid> units;

                uint32 time = 60 * IN_MILLISECONDS;

                vector<WorldPosition> ppath = l.second->getPath();

                for (auto p : ppath)
                {
                    Creature* wpCreature = bot->SummonCreature(1, p.getX(), p.getY(), p.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 20000.0f);
                    //addAura(246, wpCreature);
                    units.push_back(wpCreature->GetObjectGuid());

                    if (!start)
                        start = wpCreature;
                }

                //FakeSpell(1064, bot, start, units.front(), units, {}, pos, pos);
            }
        }
        return true;
    }
    else if (text.find("dspell ") != std::string::npos)
    {
        uint32 spellEffect = stoi(text.substr(7));

        Unit* prev = bot;

        for (float i = 0; i < 60; i++)
        {
            float ang = i / 60 * M_PI_F * 4;
            float dist = i / 60 * 30;

            WorldPosition botPos(bot);
            WorldPosition botPos1 = botPos;

            botPos.setX(botPos.getX() + cos(ang) * dist);
            botPos.setY(botPos.getY() + sin(ang) * dist);
            botPos.setZ(botPos.getHeight() + 2);

            Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

            FakeSpell(spellEffect, wpCreature, wpCreature, prev->GetObjectGuid(), {}, {}, botPos, botPos);

            prev = wpCreature;
        }
        return true;
    }
    else if (text.find("vspell ") != std::string::npos)
    {
        uint32 spellEffect = stoi(text.substr(7));

        Unit* prev = bot;

        for (float i = 0; i < 60; i++)
        {
            float ang = i / 60 * M_PI_F * 4;
            float dist = i / 60 * 30;

            WorldPosition botPos(bot);
            WorldPosition botPos1 = botPos;

            botPos.setX(botPos.getX() + cos(ang) * dist);
            botPos.setY(botPos.getY() + sin(ang) * dist);
            botPos.setZ(botPos.getHeight() + 2);

            Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);            

            if (wpCreature)
            {
                WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 8 + 4);        // visual effect on guid
                data << wpCreature->GetObjectGuid();
                data << uint32(spellEffect);                               // index from SpellVisualKit.dbc
                wpCreature->SendMessageToSet(data, true);                
            }
        }
        return true;
    }
    else if (text.find("aspell ") != std::string::npos)
    {
        uint32 spellEffect = stoi(text.substr(7));

        Unit* prev = bot;

        for (float i = 0; i < 60; i++)
        {
            float ang = i / 60 * M_PI_F * 4;
            float dist = i / 60 * 30;

            WorldPosition botPos(bot);
            WorldPosition botPos1 = botPos;

            botPos.setX(botPos.getX() + cos(ang) * dist);
            botPos.setY(botPos.getY() + sin(ang) * dist);
            botPos.setZ(botPos.getHeight() + 2);

            Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + i * 100.0f);
            wpCreature->SetObjectScale(0.5f);

            if (wpCreature)
            {
                addAura(spellEffect, wpCreature);
            }
        }
        return true;
    }
    else if (text.find("cspell ") != std::string::npos)
    {
        uint32 spellEffect = stoi(text.substr(7));

        list<ObjectGuid> units;

        for (float i = 0; i < 60; i++)
        {
            float ang = i / 60 * M_PI_F * 4;
            float dist = i / 60 * 30;

            WorldPosition botPos(bot);

            botPos.setX(botPos.getX() + cos(ang) * dist);
            botPos.setY(botPos.getY() + sin(ang) * dist);
            botPos.setZ(botPos.getHeight() + 2);

            Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

            units.push_back(wpCreature->GetObjectGuid());
        }

        WorldPosition botPos(bot);

        if(urand(0,1))
            FakeSpell(spellEffect, bot, bot, ObjectGuid(), units, {}, botPos, botPos);
        else
            FakeSpell(spellEffect, bot, bot, units.front(), units, {}, botPos, botPos);

        return true;
    }
    else if (text.find("fspell ") != std::string::npos)
    {
        uint32 spellEffect = stoi(text.substr(7));

        {
            WorldPacket data(SMSG_SPELL_START, (8 + 8 + 4 + 2 + 4));

            data.Initialize(SMSG_SPELL_START);
            data << bot->GetPackGUID();
            data << bot->GetPackGUID();
            data << uint32(spellEffect);
            data << uint16(0);
            data << uint32(0);
            data << uint16(2);
            data << master->GetObjectGuid();
            bot->SendMessageToSet(data, true);
        }

        {
            WorldPacket data(SMSG_SPELL_GO, 53);                    // guess size
            data << bot->GetPackGUID();
            data << bot->GetPackGUID();
            data << uint32(spellEffect);  // spellID
            data << uint8(0) << uint8(1);   // flags
            data << uint8(1);			   // amount of targets
            data << master->GetObjectGuid();
            data << uint8(0);
            data << uint16(2);
            data << master->GetObjectGuid();
            bot->SendMessageToSet(data, true);
        }

        return true;
    }
    else if (text.find("spell ") != std::string::npos)
    {
        uint32 spellEffect = stoi(text.substr(6));
        master->GetSession()->SendPlaySpellVisual(bot->GetObjectGuid(), spellEffect);
        return true;
    }
    else if (text.find("tspellmap") != std::string::npos)
    {
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                Creature* wpCreature = bot->SummonCreature((dy == 0 && (dx == 0 || dx == 2)) ? 6 : 2, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                if (wpCreature)
                {
                    ostringstream out;
                    out << "effect ";
                    out << effect;

                    const std::string& Cname = out.str();

                    wpCreature->MonsterSay(Cname.c_str(), 0, master);
                }
            }
        }
        return true;
    }
    else if (text.find("uspellmap") != std::string::npos)
    {
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                Creature* wpCreature = bot->SummonCreature(effect, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);
            }
        }
        return true;
    }
    else if (text.find("dspellmap") != std::string::npos)
    {
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                FakeSpell(effect, bot, nullptr, ObjectGuid(), {}, {}, botPos, botPos, true);
            }
        }
        return true;
    }
    else if (text.find("vspellmap") != std::string::npos)
    {
        vector<WorldPacket> datMap;
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                if (wpCreature)
                {
                    WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 8 + 4);        // visual effect on guid
                    data << wpCreature->GetObjectGuid();
                    data << uint32(effect); ;                               // index from SpellVisualKit.dbc
                    //wpCreature->SendMessageToSet(data, true);
                    datMap.push_back(data);

                    //wpCreature->MonsterMoveWithSpeed(botPos.getX(), botPos.getY()+80, botPos.getZ(), 8.0f,true,true);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        for (auto dat : datMap)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            bot->SendMessageToSet(dat, true);
        }
        for (auto dat : datMap)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            bot->SendMessageToSet(dat, true);
        }

        return true;
    }
    else if (text.find("ispellmap") != std::string::npos)
    {
        vector<WorldPacket> datMap;
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                Creature* wpCreature = bot->SummonCreature((dy == 0 && (dx == 0 || dx == 2)) ? 6 : 2, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                if (wpCreature)
                {
                    WorldPacket data(SMSG_PLAY_SPELL_IMPACT, 8 + 4);        // visual effect on player
                    data << wpCreature->GetObjectGuid();
                    data << uint32(effect);                                 // index from SpellVisualKit.dbc
                    //wpCreature->SendMessageToSet(data, true);
                    datMap.push_back(data);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        for (auto dat : datMap)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            bot->SendMessageToSet(dat, true);
        }
        for (auto dat : datMap)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            bot->SendMessageToSet(dat, true);
        }

        return true;
    }
    else if (text.find("cspellmap") != std::string::npos)
    {
        Creature* wpCreature = nullptr;
        Creature* lCreature = nullptr;
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                wpCreature = bot->SummonCreature((dy == 0 && (dx == 0 || dx == 2)) ? 6 : 2, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                if (wpCreature && lCreature)
                {
                    wpCreature->CastSpell(lCreature, effect, TRIGGERED_OLD_TRIGGERED);
                }

                lCreature = wpCreature;
            }
        }
        return true;
    }
    else if (text.find("aspellmap") != std::string::npos)
    {
        Creature* wpCreature = nullptr;

        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                if (wpCreature)
                {
                    addAura(effect, wpCreature);
                }
            }
        }
        return true;
    }
    else if (text.find("gspellmap") != std::string::npos)
    {
        vector<ObjectGuid> all_targets;// = { bot->GetObjectGuid(), master->GetObjectGuid() };
        //vector<ObjectGuid> all_dummies = { bot->GetObjectGuid(), master->GetObjectGuid() };

        /*list<ObjectGuid> a_targets = *context->GetValue<list<ObjectGuid> >("all targets");
        for (auto t : a_targets)
        {
            all_targets.push_back(t);
        }
        */


        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                all_targets.push_back(wpCreature->GetObjectGuid());
            }
        }

        all_targets.push_back(master->GetObjectGuid());
        all_targets.push_back(bot->GetObjectGuid());

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        for (uint32 i = 0; i < 2; i++)
            for (int32 dx = 0; dx < 10; dx++)
            {
                for (int32 dy = 0; dy < 10; dy++)
                {
                    uint32 spellEffect = stoi(text.substr(10));
                    uint32 effect = dx + dy * 10 + spellEffect * 100;

                    uint32 i = dx + dy * 10;
                    list<ObjectGuid> hits, miss;

                    SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(effect);

                    if (spellInfo && spellInfo->speed > 0)
                        for (auto tar : all_targets)
                        {
                            //hits.push_back(tar);

                            switch (urand(0, 10))
                            {
                            case 0:
                                hits.push_back(tar);
                                break;
                            case 1:
                                miss.push_back(tar);
                                break;
                            case 2:
                            case 3:
                                break;
                            }
                        }

                    Unit* realCaster = ai->GetUnit(all_targets[i]);//ai->GetUnit(all_targets[urand(0, all_targets.size() - 1)]);
                    Unit* caster = ai->GetUnit(all_targets[i]);//ai->GetUnit(all_targets[urand(0, all_targets.size() - 1)]);
                    Unit* target = ai->GetUnit(all_targets[i + 1]);

                    if (!realCaster)
                        realCaster = bot;

                    if (!caster)
                        caster = bot;

                    if (!target)
                        target = master;

                    FakeSpell(effect, realCaster, caster, target->GetObjectGuid(), hits, miss, WorldPosition(caster), WorldPosition(target));

                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }
            }
        return true;
    }
    else if (text.find("mspellmap") != std::string::npos)
    {
    vector<ObjectGuid> all_targets;

    for (int32 dx = 0; dx < 10; dx++)
    {
        for (int32 dy = 0; dy < 10; dy++)
        {
            WorldPosition botPos(bot);

            botPos.setX(botPos.getX() + (dx - 5) * 5);
            botPos.setY(botPos.getY() + (dy - 5) * 5);
            botPos.setZ(botPos.getHeight());

            Creature* wpCreature = bot->SummonCreature(2334, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

            all_targets.push_back(wpCreature->GetObjectGuid());
        }
    }

    all_targets.push_back(master->GetObjectGuid());
    all_targets.push_back(bot->GetObjectGuid());

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (uint32 i = 0; i < 2; i++)
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 spellEffect = stoi(text.substr(10));
                uint32 effect = dx + dy * 10 + spellEffect * 100;

                uint32 i = dx + dy * 10;
                list<ObjectGuid> hits, miss;

                SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(effect);

                    for (auto tar : all_targets)
                    {
                        //hits.push_back(tar);

                        switch (urand(0, 10))
                        {
                        case 0:
                            hits.push_back(tar);
                            break;
                        case 1:
                            miss.push_back(tar);
                            break;
                        case 2:
                        case 3:
                            break;
                        }
                    }

                Unit* realCaster = ai->GetUnit(all_targets[i]);//ai->GetUnit(all_targets[urand(0, all_targets.size() - 1)]);
                Unit* caster = ai->GetUnit(all_targets[i]);//ai->GetUnit(all_targets[urand(0, all_targets.size() - 1)]);
                Unit* target = ai->GetUnit(all_targets[i + 1]);

                if (!realCaster)
                    realCaster = bot;

                if (!caster)
                    caster = bot;

                if (!target)
                    target = master;

                master->GetSession()->SendPlaySpellVisual(caster->GetObjectGuid(), 5036);
                FakeSpell(effect, realCaster, caster, target->GetObjectGuid(), hits, miss, WorldPosition(caster), WorldPosition(target));

                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
    return true;
    }
    else if (text.find("soundmap") != std::string::npos)
    {
        uint32 soundEffects = stoi(text.substr(9));
        for (int32 dx = 0; dx < 10; dx++)
        {
            for (int32 dy = 0; dy < 10; dy++)
            {
                uint32 effect = dx + dy * 10 + soundEffects * 100;
                WorldPosition botPos(bot);

                botPos.setX(botPos.getX() + (dx - 5) * 5);
                botPos.setY(botPos.getY() + (dy - 5) * 5);
                botPos.setZ(botPos.getHeight());

                Creature* wpCreature = bot->SummonCreature((dy == 0 && (dx == 0 || dx == 2)) ? 6 : 2, botPos.getX(), botPos.getY(), botPos.getZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 10000.0f);

                wpCreature->PlayDistanceSound(effect);
            }
        }
    }
    else if (text.find("sounds") != std::string::npos)
    {
        uint32 soundEffects = stoi(text.substr(6));

        for (uint32 i = 0; i < 100; i++)
        {
            bot->PlayDistanceSound(i + soundEffects * 100);
            bot->Say(to_string(i + soundEffects * 100), 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        return true;
    }
    else if (text.find("dsound") != std::string::npos)
    {
        uint32 soundEffect = stoi(text.substr(7));
        bot->PlayDirectSound(soundEffect);
        return true;
    }
    else if (text.find("dsound") != std::string::npos)
    {
        uint32 soundEffect = stoi(text.substr(7));
        bot->PlayDirectSound(soundEffect);
        return true;
    }
    else if (text.find("sound") != std::string::npos)
    {
        uint32 soundEffect = stoi(text.substr(6));
        bot->PlayDistanceSound(soundEffect);
        return true;
    }

    string response = ai->HandleRemoteCommand(text);
    ai->TellMaster(response);
    return true;
}


void DebugAction::FakeSpell(uint32 spellId, Unit* truecaster, Unit* caster, ObjectGuid target, list<ObjectGuid> otherTargets, list<ObjectGuid> missTargets, WorldPosition source, WorldPosition dest, bool forceDest)
{
    SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(spellId);
    {
        uint32 castFlags = CAST_FLAG_UNKNOWN2;

        if (spellInfo && spellInfo->HasAttribute(SPELL_ATTR_USES_RANGED_SLOT))
            castFlags |= CAST_FLAG_AMMO;                        // arrows/bullets visual

        WorldPacket data(SMSG_SPELL_START, (8 + 8 + 4 + 2 + 4));

        data << truecaster->GetPackGUID();                      //truecaster

        if (caster)
            data << caster->GetPackGUID();                          //m_caster->GetPackGUID();     
        else
            data << ObjectGuid();

        data << uint32(spellId);                                // spellId
        data << uint16(castFlags);                              // cast flags
        data << uint32(1000.0f);                                   // delay?

        SpellCastTargets m_targets;

        data << m_targets;

        //projectile info
        if (castFlags & CAST_FLAG_AMMO)
        {
            data << uint32(5996);
            data << uint32(INVTYPE_AMMO);
        }

        if(caster)
            caster->GetMap()->MessageBroadcast(caster, data);
        else
            truecaster->GetMap()->MessageBroadcast(truecaster, data);
    }
    {
        uint32 castFlags = CAST_FLAG_UNKNOWN9;



        if (spellInfo && spellInfo->HasAttribute(SPELL_ATTR_USES_RANGED_SLOT))
            castFlags |= CAST_FLAG_AMMO;                        // arrows/bullets visual
        if (spellInfo && HasPersistentAuraEffect(spellInfo))
            castFlags |= CAST_FLAG_PERSISTENT_AA;

        WorldPacket data(SMSG_SPELL_GO, 53);                    // guess size

        data << truecaster->GetPackGUID();                      //truecaster

        if (caster)
            data << caster->GetPackGUID();                          //m_caster->GetPackGUID();     
        else
            data << ObjectGuid();

        data << uint32(spellId);                                // spellId
        data << uint16(castFlags);                              // cast flags

        //WriteSpellGoTargets
        uint32 hits = otherTargets.size() + (target ? 1 : 0);

        data << uint8(hits);                                      // Hits                   

        if (target)
            data << target;

        //Hit targets here.                   
        for (auto otherTarget : otherTargets)
            data << otherTarget;

        data << (uint8)missTargets.size();                         //miss

        for (auto missTarget : missTargets)
        {
            data << missTarget;

            data << uint8(SPELL_MISS_RESIST);    //Miss condition
            data << uint8(SPELL_MISS_NONE);    //Miss condition
        }

        SpellCastTargets m_targets;

        if ((spellInfo && spellInfo->Targets & TARGET_FLAG_DEST_LOCATION) || forceDest)
            m_targets.setDestination(dest.getX(), dest.getY(), dest.getZ());
        if ((spellInfo && spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION) || forceDest)
            m_targets.setSource(source.getX(), source.getY(), source.getZ());
        if (!forceDest && target)
            if(!spellInfo || !(spellInfo->Targets & TARGET_FLAG_DEST_LOCATION && spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION))
                m_targets.setUnitTarget(ai->GetUnit(target));

        data << m_targets;

        //projectile info
        if (castFlags & CAST_FLAG_AMMO)
        {
            data << uint32(5996);
            data << uint32(INVTYPE_AMMO);
        }

        if (caster)
            caster->GetMap()->MessageBroadcast(caster, data);
        else
            truecaster->GetMap()->MessageBroadcast(truecaster, data);
    }
}

void DebugAction::addAura(uint32 spellId, Unit* target)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return;

    if (!IsSpellAppliesAura(spellInfo, (1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) &&
        !IsSpellHaveEffect(spellInfo, SPELL_EFFECT_PERSISTENT_AREA_AURA))
    {
        return;
    }

    SpellAuraHolder* holder = CreateSpellAuraHolder(spellInfo, target, target);

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
            int32 damage = 0; // no damage cos caster doesnt exist
            Aura* aur = CreateAura(spellInfo, SpellEffectIndex(i), &damage, &basePoints, holder, target);
            holder->AddAura(aur, SpellEffectIndex(i));
        }
    }
    if (!target->AddSpellAuraHolder(holder))
        delete holder;

    return;
}