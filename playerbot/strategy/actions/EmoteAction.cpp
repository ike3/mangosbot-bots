#include "botpch.h"
#include "../../playerbot.h"
#include "EmoteAction.h"

#include "../../PlayerbotAIConfig.h"
using namespace ai;

map<string, uint32> EmoteAction::emotes;
map<string, uint32> EmoteAction::textEmotes;

bool EmoteAction::Execute(Event event)
{
    if (emotes.empty())
        InitEmotes();

    uint32 emote = 0;

    string param = event.getParam();
    if (param.empty())
    {
        time_t lastEmote = AI_VALUE2(time_t, "last emote", qualifier);
        ai->GetAiObjectContext()->GetValue<time_t>("last emote", qualifier)->Set(time(0) + urand(1000, sPlayerbotAIConfig.repeatDelay) / 1000);
    }

    if (param.empty()) param = qualifier;

    if (param.find("sound") == 0)
    {
        return ai->PlaySound(atoi(param.substr(5).c_str()));
    }

    if (!param.empty() && textEmotes.find(param) != textEmotes.end())
    {
        return ai->PlaySound(textEmotes[param]);
    }

    if (param.empty() || emotes.find(param) == emotes.end())
    {
        int index = rand() % emotes.size();
		for (map<string, uint32>::iterator i = emotes.begin(); i != emotes.end() && index; ++i, --index)
			emote = i->second;
    }
    else
    {
        emote = emotes[param];
    }

    if (param.find("text") == 0)
    {
        emote = atoi(param.substr(4).c_str());
    }

    Player* master = GetMaster();
	if (master)
	{
        ObjectGuid masterSelection = master->GetSelectionGuid();
        if (masterSelection)
        {
            ObjectGuid oldSelection = bot->GetSelectionGuid();
            bot->SetSelectionGuid(masterSelection);
            bot->HandleEmoteCommand(emote);
            if (oldSelection)
                bot->SetSelectionGuid(oldSelection);
            return true;
        }
	}

    bot->HandleEmoteCommand(emote);
    return true;
}

void EmoteAction::InitEmotes()
{
    emotes["dance"] = EMOTE_ONESHOT_DANCE;
    emotes["drown"] = EMOTE_ONESHOT_DROWN;
    emotes["land"] = EMOTE_ONESHOT_LAND;
    emotes["liftoff"] = EMOTE_ONESHOT_LIFTOFF;
    emotes["loot"] = EMOTE_ONESHOT_LOOT;
    emotes["no"] = EMOTE_ONESHOT_NO;
    emotes["roar"] = EMOTE_STATE_ROAR;
    emotes["salute"] = EMOTE_ONESHOT_SALUTE;
    emotes["stomp"] = EMOTE_ONESHOT_STOMP;
    emotes["train"] = EMOTE_ONESHOT_TRAIN;
    emotes["yes"] = EMOTE_ONESHOT_YES;
    emotes["applaud"] = EMOTE_ONESHOT_APPLAUD;
    emotes["beg"] = EMOTE_ONESHOT_BEG;
    emotes["bow"] = EMOTE_ONESHOT_BOW;
    emotes["cheer"] = EMOTE_ONESHOT_CHEER;
    emotes["chicken"] = EMOTE_ONESHOT_CHICKEN;
    emotes["cry"] = EMOTE_ONESHOT_CRY;
    emotes["dance"] = EMOTE_STATE_DANCE;
    emotes["eat"] = EMOTE_ONESHOT_EAT;
    emotes["exclamation"] = EMOTE_ONESHOT_EXCLAMATION;
    emotes["flex"] = EMOTE_ONESHOT_FLEX;
    emotes["kick"] = EMOTE_ONESHOT_KICK;
    emotes["kiss"] = EMOTE_ONESHOT_KISS;
    emotes["kneel"] = EMOTE_ONESHOT_KNEEL;
    emotes["laugh"] = EMOTE_ONESHOT_LAUGH;
    emotes["point"] = EMOTE_ONESHOT_POINT;
    emotes["question"] = EMOTE_ONESHOT_QUESTION;
    emotes["ready1h"] = EMOTE_ONESHOT_READY1H;
    emotes["roar"] = EMOTE_ONESHOT_ROAR;
    emotes["rude"] = EMOTE_ONESHOT_RUDE;
    emotes["shout"] = EMOTE_ONESHOT_SHOUT;
    emotes["shy"] = EMOTE_ONESHOT_SHY;
    emotes["sleep"] = EMOTE_STATE_SLEEP;
    emotes["talk"] = EMOTE_ONESHOT_TALK;
    emotes["wave"] = EMOTE_ONESHOT_WAVE;
    emotes["wound"] = EMOTE_ONESHOT_WOUND;

    textEmotes["bored"] = TEXTEMOTE_BORED;
    textEmotes["bye"] = TEXTEMOTE_BYE;
    textEmotes["cheer"] = TEXTEMOTE_CHEER;
    textEmotes["congratulate"] = TEXTEMOTE_CONGRATULATE;
    textEmotes["hello"] = TEXTEMOTE_HELLO;
    textEmotes["no"] = TEXTEMOTE_NO;
    textEmotes["nod"] = TEXTEMOTE_NOD; // yes
    textEmotes["sigh"] = TEXTEMOTE_SIGH;
    textEmotes["thank"] = TEXTEMOTE_THANK;
    textEmotes["welcome"] = TEXTEMOTE_WELCOME; // you are welcome
    textEmotes["whistle"] = TEXTEMOTE_WHISTLE;
    textEmotes["yawn"] = TEXTEMOTE_YAWN;
    textEmotes["oom"] = 323;
    textEmotes["follow"] = 324;
    textEmotes["wait"] = 325;
    textEmotes["healme"] = 326;
    textEmotes["openfire"] = 327;
    textEmotes["helpme"] = 303;
}


bool EmoteAction::isUseful()
{
    time_t lastEmote = AI_VALUE2(time_t, "last emote", qualifier);
    return (time(0) - lastEmote) >= sPlayerbotAIConfig.repeatDelay / 1000;
}
