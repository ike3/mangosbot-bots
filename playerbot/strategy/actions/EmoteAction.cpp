#include "botpch.h"
#include "../../playerbot.h"
#include "EmoteAction.h"

#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
using namespace ai;

map<string, uint32> EmoteActionBase::emotes;
map<string, uint32> EmoteActionBase::textEmotes;
char *strstri(const char *haystack, const char *needle);

EmoteActionBase::EmoteActionBase(PlayerbotAI* ai, string name) : Action(ai, name)
{
    if (emotes.empty()) InitEmotes();
}

EmoteAction::EmoteAction(PlayerbotAI* ai) : EmoteActionBase(ai, "emote"), Qualified()
{
}

void EmoteActionBase::InitEmotes()
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
    textEmotes["flee"] = 306;
    textEmotes["danger"] = 304;
    textEmotes["charge"] = 305;
    textEmotes["help"] = 307;
    textEmotes["train"] = 264;
}

bool EmoteActionBase::Emote(Unit* target, uint32 type)
{
    if (sServerFacade.isMoving(bot)) return false;

    if (target && !sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, EMOTE_ANGLE_IN_FRONT))
        sServerFacade.SetFacingTo(bot, target);

    ObjectGuid oldSelection = bot->GetSelectionGuid();
    if (target)
    {
        bot->SetSelectionGuid(target->GetObjectGuid());
        Player* player = dynamic_cast<Player*>(target);
        if (player && player->GetPlayerbotAI() && !sServerFacade.IsInFront(player, bot, sPlayerbotAIConfig.sightDistance, EMOTE_ANGLE_IN_FRONT))
            sServerFacade.SetFacingTo(player, bot);
    }

    bot->HandleEmoteCommand(type);

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    return true;
}

Unit* EmoteActionBase::GetTarget()
{
    Unit* target = NULL;

    list<ObjectGuid> nfp = *context->GetValue<list<ObjectGuid> >("nearest friendly players");
    vector<Unit*> targets;
    for (list<ObjectGuid>::iterator i = nfp.begin(); i != nfp.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && sServerFacade.GetDistance2d(bot, unit) < sPlayerbotAIConfig.tooCloseDistance) targets.push_back(unit);
    }

    if (!targets.empty())
        target = targets[urand(0, targets.size() - 1)];

    return target;
}

bool EmoteActionBase::ReceiveEmote(Player* source, uint32 emote)
{
    uint32 emoteId = 0;
    string emoteText;
    string emoteYell;
    switch (emote)
    {
    case TEXTEMOTE_BONK:
        emoteId = EMOTE_ONESHOT_CRY;
        break;
    case TEXTEMOTE_SALUTE:
        emoteId = EMOTE_ONESHOT_SALUTE;
        break;
    case 325:
        if (ai->GetMaster() == source)
        {
            ai->ChangeStrategy("-follow,+stay", BOT_STATE_NON_COMBAT);
            ai->TellMasterNoFacing("Fine.. I'll stay right here..");
        }
        break;
    case TEXTEMOTE_BECKON:
    case 324:
        if (ai->GetMaster() == source)
        {
            ai->ChangeStrategy("+follow", BOT_STATE_NON_COMBAT);
            ai->TellMasterNoFacing("Wherever you go, I'll follow..");
        }
        break;
    case TEXTEMOTE_WAVE:
    case TEXTEMOTE_GREET:
    case TEXTEMOTE_HAIL:
    case TEXTEMOTE_HELLO:
    case TEXTEMOTE_WELCOME:
    case TEXTEMOTE_INTRODUCE:
        emoteText = "Hey there!";
        emoteId = EMOTE_ONESHOT_WAVE;
        break;
    case TEXTEMOTE_DANCE:
        emoteText = "Shake what your mama gave you!";
        emoteId = EMOTE_ONESHOT_DANCE;
        break;
    case TEXTEMOTE_FLIRT:
    case TEXTEMOTE_KISS:
    case TEXTEMOTE_HUG:
    case TEXTEMOTE_BLUSH:
    case TEXTEMOTE_SMILE:
    case TEXTEMOTE_LOVE:
        //case TEXTEMOTE_HOLDHAND:
        emoteText = "Awwwww...";
        emoteId = EMOTE_ONESHOT_SHY;
        break;
    case TEXTEMOTE_FLEX:
        emoteText = "Hercules! Hercules!";
        emoteId = EMOTE_ONESHOT_APPLAUD;
        break;
    case TEXTEMOTE_ANGRY:
        //case TEXTEMOTE_FACEPALM:
    case TEXTEMOTE_GLARE:
    case TEXTEMOTE_BLAME:
        //case TEXTEMOTE_FAIL:
        //case TEXTEMOTE_REGRET:
        //case TEXTEMOTE_SCOLD:
        //case TEXTEMOTE_CROSSARMS:
        emoteText = "Did I do thaaaaat?";
        emoteId = EMOTE_ONESHOT_QUESTION;
        break;
    case TEXTEMOTE_FART:
    case TEXTEMOTE_BURP:
    case TEXTEMOTE_GASP:
    case TEXTEMOTE_NOSEPICK:
    case TEXTEMOTE_SNIFF:
    case TEXTEMOTE_STINK:
        emoteText = "Wasn't me! Just sayin'..";
        emoteId = EMOTE_ONESHOT_POINT;
        break;
    case TEXTEMOTE_JOKE:
        emoteId = EMOTE_ONESHOT_LAUGH;
        emoteText = "Oh.. was I not supposed to laugh so soon?";
        break;
    case TEXTEMOTE_CHICKEN:
        emoteText = "We'll see who's chicken soon enough!";
        emoteId = EMOTE_ONESHOT_RUDE;
        break;
    case TEXTEMOTE_APOLOGIZE:
        emoteId = EMOTE_ONESHOT_POINT;
        emoteText = "You damn right you're sorry!";
        break;
    case TEXTEMOTE_APPLAUD:
    case TEXTEMOTE_CLAP:
    case TEXTEMOTE_CONGRATULATE:
    case TEXTEMOTE_HAPPY:
        //case TEXTEMOTE_GOLFCLAP:
        emoteId = EMOTE_ONESHOT_BOW;
        emoteText = "Thank you.. Thank you.. I'm here all week.";
        break;
    case TEXTEMOTE_BEG:
    case TEXTEMOTE_GROVEL:
    case TEXTEMOTE_PLEAD:
        emoteId = EMOTE_ONESHOT_NO;
        emoteText = "Beg all you want.. I have nothing for you.";
        break;
    case TEXTEMOTE_BITE:
    case TEXTEMOTE_POKE:
    case TEXTEMOTE_SCRATCH:
        //case TEXTEMOTE_PINCH:
        //case TEXTEMOTE_PUNCH:
        emoteId = EMOTE_ONESHOT_ROAR;
        emoteYell = "OUCH! Dammit, that hurt!";
        break;
    case TEXTEMOTE_BORED:
        emoteId = EMOTE_ONESHOT_NO;
        emoteText = "My job description doesn't include entertaining you..";
        break;
    case TEXTEMOTE_BOW:
    case TEXTEMOTE_CURTSEY:
        emoteId = EMOTE_ONESHOT_BOW;
        break;
    case TEXTEMOTE_BRB:
    case TEXTEMOTE_SIT:
        emoteId = EMOTE_ONESHOT_EAT;
        emoteText = "Looks like time for an AFK break..";
        break;
    case TEXTEMOTE_AGREE:
    case TEXTEMOTE_NOD:
        emoteId = EMOTE_ONESHOT_EXCLAMATION;
        emoteText = "At least SOMEONE agrees with me!";
        break;
    case TEXTEMOTE_AMAZE:
    case TEXTEMOTE_COWER:
    case TEXTEMOTE_CRINGE:
    case TEXTEMOTE_EYE:
    case TEXTEMOTE_KNEEL:
    case TEXTEMOTE_PEER:
    case TEXTEMOTE_SURRENDER:
    case TEXTEMOTE_PRAISE:
    case TEXTEMOTE_SCARED:
    case TEXTEMOTE_COMMEND:
        //case TEXTEMOTE_AWE:
        //case TEXTEMOTE_JEALOUS:
        //case TEXTEMOTE_PROUD:
        emoteId = EMOTE_ONESHOT_FLEX;
        emoteText = "Yes, Yes. I know I'm amazing..";
        break;
    case TEXTEMOTE_BLEED:
    case TEXTEMOTE_MOURN:
    case TEXTEMOTE_FLOP:
        //case TEXTEMOTE_FAINT:
        //case TEXTEMOTE_PULSE:
        emoteId = EMOTE_ONESHOT_KNEEL;
        emoteText = "MEDIC! Stat!";
        break;
    case TEXTEMOTE_BLINK:
        emoteId = EMOTE_ONESHOT_KICK;
        emoteText = "What? You got something in your eye?";
        break;
    case TEXTEMOTE_BOUNCE:
    case TEXTEMOTE_BARK:
        emoteId = EMOTE_ONESHOT_POINT;
        emoteText = "Who's a good doggy? You're a good doggy!";
        break;
    case TEXTEMOTE_BYE:
        emoteId = EMOTE_ONESHOT_WAVE;
        emoteText = "Umm.... wait! Where are you going?!";
        break;
    case TEXTEMOTE_CACKLE:
    case TEXTEMOTE_LAUGH:
    case TEXTEMOTE_CHUCKLE:
    case TEXTEMOTE_GIGGLE:
    case TEXTEMOTE_GUFFAW:
    case TEXTEMOTE_ROFL:
    case TEXTEMOTE_SNICKER:
        //case TEXTEMOTE_SNORT:
        emoteId = EMOTE_ONESHOT_LAUGH;
        emoteText = "Wait... what are we laughing at again?";
        break;
    case TEXTEMOTE_CONFUSED:
    case TEXTEMOTE_CURIOUS:
    case TEXTEMOTE_FIDGET:
    case TEXTEMOTE_FROWN:
    case TEXTEMOTE_SHRUG:
    case TEXTEMOTE_SIGH:
    case TEXTEMOTE_STARE:
    case TEXTEMOTE_TAP:
    case TEXTEMOTE_SURPRISED:
    case TEXTEMOTE_WHINE:
    case TEXTEMOTE_BOGGLE:
    case TEXTEMOTE_LOST:
    case TEXTEMOTE_PONDER:
    case TEXTEMOTE_SNUB:
    case TEXTEMOTE_SERIOUS:
    case TEXTEMOTE_EYEBROW:
        emoteId = EMOTE_ONESHOT_QUESTION;
        emoteText = "Don't look at  me.. I just work here";
        break;
    case TEXTEMOTE_COUGH:
    case TEXTEMOTE_DROOL:
    case TEXTEMOTE_SPIT:
    case TEXTEMOTE_LICK:
    case TEXTEMOTE_BREATH:
        //case TEXTEMOTE_SNEEZE:
        //case TEXTEMOTE_SWEAT:
        emoteId = EMOTE_ONESHOT_POINT;
        emoteText = "Ewww! Keep your nasty germs over there!";
        break;
    case TEXTEMOTE_CRY:
        emoteId = EMOTE_ONESHOT_CRY;
        emoteText = "Don't you start crying or it'll make me start crying!";
        break;
    case TEXTEMOTE_CRACK:
        emoteId = EMOTE_ONESHOT_ROAR;
        emoteText = "It's clobbering time!";
        break;
    case TEXTEMOTE_EAT:
    case TEXTEMOTE_DRINK:
        emoteId = EMOTE_ONESHOT_EAT;
        emoteText = "I hope you brought enough for the whole class...";
        break;
    case TEXTEMOTE_GLOAT:
    case TEXTEMOTE_MOCK:
    case TEXTEMOTE_TEASE:
    case TEXTEMOTE_EMBARRASS:
        emoteId = EMOTE_ONESHOT_CRY;
        emoteText = "Doesn't mean you need to be an ass about it..";
        break;
    case TEXTEMOTE_HUNGRY:
        emoteId = EMOTE_ONESHOT_EAT;
        emoteText = "What? You want some of this?";
        break;
    case TEXTEMOTE_LAYDOWN:
    case TEXTEMOTE_TIRED:
    case TEXTEMOTE_YAWN:
        emoteId = EMOTE_ONESHOT_KNEEL;
        emoteText = "Is it break time already?";
        break;
    case TEXTEMOTE_MOAN:
    case TEXTEMOTE_MOON:
    case TEXTEMOTE_SEXY:
    case TEXTEMOTE_SHAKE:
    case TEXTEMOTE_WHISTLE:
    case TEXTEMOTE_CUDDLE:
    case TEXTEMOTE_PURR:
    case TEXTEMOTE_SHIMMY:
    case TEXTEMOTE_SMIRK:
    case TEXTEMOTE_WINK:
        //case TEXTEMOTE_CHARM:
        emoteId = EMOTE_ONESHOT_NO;
        emoteText = "Keep it in your pants, boss..";
        break;
    case TEXTEMOTE_NO:
    case TEXTEMOTE_VETO:
    case TEXTEMOTE_DISAGREE:
    case TEXTEMOTE_DOUBT:
        emoteId = EMOTE_ONESHOT_QUESTION;
        emoteText = "Aww.... why not?!";
        break;
    case TEXTEMOTE_PANIC:
        emoteId = EMOTE_ONESHOT_EXCLAMATION;
        emoteText = "Now is NOT the time to panic!";
        break;
    case TEXTEMOTE_POINT:
        emoteId = EMOTE_ONESHOT_POINT;
        emoteText = "What?! I can do that TOO!";
        break;
    case TEXTEMOTE_RUDE:
    case TEXTEMOTE_RASP:
        emoteId = EMOTE_ONESHOT_RUDE;
        emoteText = "Right back at you, bub!", LANG_UNIVERSAL;
        break;
    case TEXTEMOTE_ROAR:
    case TEXTEMOTE_THREATEN:
    case TEXTEMOTE_CALM:
    case TEXTEMOTE_DUCK:
    case TEXTEMOTE_TAUNT:
    case TEXTEMOTE_PITY:
    case TEXTEMOTE_GROWL:
        //case TEXTEMOTE_TRAIN:
        //case TEXTEMOTE_INCOMING:
        //case TEXTEMOTE_CHARGE:
        //case TEXTEMOTE_FLEE:
        //case TEXTEMOTE_ATTACKMYTARGET:
    case TEXTEMOTE_OPENFIRE:
    case TEXTEMOTE_ENCOURAGE:
    case TEXTEMOTE_ENEMY:
        //case TEXTEMOTE_CHALLENGE:
        //case TEXTEMOTE_REVENGE:
        //case TEXTEMOTE_SHAKEFIST:
        emoteId = EMOTE_ONESHOT_ROAR;
        emoteYell = "RAWR!";
        break;
    case TEXTEMOTE_TALK:
    case TEXTEMOTE_TALKEX:
    case TEXTEMOTE_TALKQ:
    case TEXTEMOTE_LISTEN:
        emoteId = EMOTE_ONESHOT_TALK;
        emoteText = "Blah Blah Blah Yakety Smackety..";
        break;
    case TEXTEMOTE_THANK:
        emoteId = EMOTE_ONESHOT_BOW;
        emoteText = "You are quite welcome!";
        break;
    case TEXTEMOTE_VICTORY:
    case TEXTEMOTE_CHEER:
    case TEXTEMOTE_TOAST:
        //case TEXTEMOTE_HIGHFIVE:
        //case TEXTEMOTE_DING:
        emoteId = EMOTE_ONESHOT_CHEER;
        emoteText = "Yay!";
        break;
    case TEXTEMOTE_COLD:
    case TEXTEMOTE_SHIVER:
    case TEXTEMOTE_THIRSTY:
        //case TEXTEMOTE_OOM:
        //case TEXTEMOTE_HEALME:
        //case TEXTEMOTE_POUT:
        emoteId = EMOTE_ONESHOT_QUESTION;
        emoteText = "And what exactly am I supposed to do about that?";
        break;
    case TEXTEMOTE_COMFORT:
    case TEXTEMOTE_SOOTHE:
    case TEXTEMOTE_PAT:
        emoteId = EMOTE_ONESHOT_CRY;
        emoteText = "Thanks...";
        break;
    case TEXTEMOTE_INSULT:
        emoteId = EMOTE_ONESHOT_CRY;
        emoteText = "You hurt my feelings..";
        break;
    case TEXTEMOTE_JK:
        emoteId = EMOTE_ONESHOT_POINT;
        emoteText = "You.....";
        break;
    case TEXTEMOTE_RAISE:
        emoteId = EMOTE_ONESHOT_POINT;
        emoteText = "Yes.. you.. at the back of the class..";
        break;
    case TEXTEMOTE_READY:
        emoteId = EMOTE_ONESHOT_SALUTE;
        emoteText = "Ready here, too!";
        break;
    case TEXTEMOTE_SHOO:
        emoteId = EMOTE_ONESHOT_KICK;
        emoteText = "Shoo yourself!";
        break;
    case TEXTEMOTE_SLAP:
        //case TEXTEMOTE_SMACK:
        emoteId = EMOTE_ONESHOT_CRY;
        emoteText = "What did I do to deserve that?";
        break;
    case TEXTEMOTE_STAND:
        emoteId = EMOTE_ONESHOT_NONE;
        emoteText = "What? Break time's over? Fine..";
        break;
    case TEXTEMOTE_TICKLE:
        emoteId = EMOTE_ONESHOT_LAUGH;
        emoteText = "Hey! Stop that!";
        break;
    case TEXTEMOTE_VIOLIN:
        emoteId = EMOTE_ONESHOT_TALK;
        emoteText = "Har Har.. very funny..";
        break;
        //case TEXTEMOTE_HELPME:
        //    bot->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
        //    bot->Yell("Quick! Someone HELP!", LANG_UNIVERSAL);
        //    break;
    case TEXTEMOTE_GOODLUCK:
        //case TEXTEMOTE_LUCK:
        emoteId = EMOTE_ONESHOT_TALK;
        emoteText = "Thanks... I'll need it..";
        break;
    case TEXTEMOTE_BRANDISH:
        //case TEXTEMOTE_MERCY:
        emoteId = EMOTE_ONESHOT_BEG;
        emoteText = "Please don't kill me!";
        break;
        /*case TEXTEMOTE_BADFEELING:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
            bot->Say("I'm just waiting for the ominous music now...", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_MAP:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_NO);
            bot->Say("Noooooooo.. you just couldn't ask for directions, huh?", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_IDEA:
        case TEXTEMOTE_THINK:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_NO);
            bot->Say("Oh boy.. another genius idea...", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_OFFER:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_NO);
            bot->Say("No thanks.. I had some back at the last village", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_PET:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
            bot->Say("Do I look like a dog to you?!", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_ROLLEYES:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
            bot->Say("Keep doing that and I'll roll those eyes right out of your head..", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_SING:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
            bot->Say("Lovely... just lovely..", LANG_UNIVERSAL);
            break;
        case TEXTEMOTE_COVEREARS:
            bot->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
            bot->Yell("You think that's going to help you?!", LANG_UNIVERSAL);
            break;*/
    default:
        //return false;
        //bot->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
        //bot->Say("Mmmmmkaaaaaay...", LANG_UNIVERSAL);
        break;
    }

    if (source && !sServerFacade.isMoving(bot) && !sServerFacade.IsInFront(bot, source, sPlayerbotAIConfig.sightDistance, EMOTE_ANGLE_IN_FRONT))
        sServerFacade.SetFacingTo(bot, source);

    if (emoteText.size())
        bot->Say(emoteText, (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));

    if (emoteYell.size())
        bot->Yell(emoteYell, (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));

    if (emoteId)
        bot->HandleEmoteCommand(emoteId);

    return true;
}

bool EmoteAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    uint32 emote = 0;
    Player* pSource = NULL;
    bool isReact = false;
    if (!p.empty() && p.GetOpcode() == SMSG_TEXT_EMOTE)
    {
        isReact = true;
        ObjectGuid source;
        uint32 text_emote;
        uint32 emote_num;
        uint32 namlen;
        string nam;
        p.rpos(0);
        p >> source >> text_emote >> emote_num >> namlen;
        if (namlen > 1)
            p.read(nam, namlen);

        if (strstri(bot->GetName(), nam.c_str()))
        {
            pSource = sObjectMgr.GetPlayer(source);

            if (pSource && sServerFacade.GetDistance2d(bot, pSource) < sPlayerbotAIConfig.farDistance)
            {
                sLog.outDetail("Bot #%d %s:%d <%s> received SMSG_TEXT_EMOTE %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), text_emote);
                emote = text_emote;
            }

        }
    }

    if (!p.empty() && p.GetOpcode() == SMSG_EMOTE)
    {
        isReact = true;
        ObjectGuid source;
        uint32 emoteId;
        p.rpos(0);
        p >> emoteId >> source;

        pSource = sObjectMgr.GetPlayer(source);
        if (pSource && sServerFacade.GetDistance2d(bot, pSource) < sPlayerbotAIConfig.farDistance && emoteId != EMOTE_ONESHOT_NONE)
        {
            if (pSource->GetSelectionGuid() == bot->GetObjectGuid())
            {
                sLog.outDetail("Bot #%d %s:%d <%s> received SMSG_EMOTE %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), emoteId);
                vector<uint32> types;
                for (int32 i = sEmotesTextStore.GetNumRows(); i >= 0; --i)
                {
                    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(uint32(i));
                    if (!em)
                        continue;

                    if (em->textid == EMOTE_ONESHOT_TALK)
                        continue;

                    if (em->textid == EMOTE_ONESHOT_QUESTION)
                        continue;

                    if(em->textid == EMOTE_ONESHOT_EXCLAMATION)
                        continue;

                    if (em->textid == emoteId)
                    {
                        types.push_back(em->Id);
                    }
                }
                if (types.size())
                    emote = types[urand(0, types.size() - 1)];
            }
        }
    }

    if (isReact && !emote)
        return false;

    string param = event.getParam();
    if ((!isReact && param.empty()) || emote)
    {
        time_t lastEmote = AI_VALUE2(time_t, "last emote", qualifier);
        ai->GetAiObjectContext()->GetValue<time_t>("last emote", qualifier)->Set(time(0) + urand(1000, sPlayerbotAIConfig.repeatDelay) / 1000);
        param = qualifier;
    }

    if (emote)
        return ReceiveEmote(pSource, emote);

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

    return Emote(GetTarget(), emote);
}

bool EmoteAction::isUseful()
{
    if (!ai->HasPlayerNearby())
        return false;

    if (sServerFacade.isMoving(bot))
        return false;

    time_t lastEmote = AI_VALUE2(time_t, "last emote", qualifier);
    return (time(0) - lastEmote) >= sPlayerbotAIConfig.repeatDelay / 1000;
}


bool TalkAction::Execute(Event event)
{
    Unit* target = ai->GetUnit(AI_VALUE(ObjectGuid, "talk target"));
    if (!target)
        target = GetTarget();

    if (!urand(0, 100))
    {
        target = NULL;
        context->GetValue<ObjectGuid>("talk target")->Set(ObjectGuid());
        return true;
    }

    if (target)
    {
        Player* player = dynamic_cast<Player*>(target);
        if (player && player->GetPlayerbotAI())
            player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("talk target")->Set(bot->GetObjectGuid());

        context->GetValue<ObjectGuid>("talk target")->Set(target->GetObjectGuid());
        return Emote(target, GetRandomEmote(target));
    }

    return false;
}

uint32 TalkAction::GetRandomEmote(Unit* unit)
{
    vector<uint32> types;
    if (!urand(0, 20))
    {
        // expressions
        types.push_back(EMOTE_ONESHOT_BOW);
        types.push_back(EMOTE_ONESHOT_RUDE);
        types.push_back(EMOTE_ONESHOT_CRY);
        types.push_back(EMOTE_ONESHOT_LAUGH);
        types.push_back(EMOTE_ONESHOT_POINT);
        types.push_back(EMOTE_ONESHOT_CHEER);
        types.push_back(EMOTE_ONESHOT_SHY);
    }
    else
    {
        // talk
        types.push_back(EMOTE_ONESHOT_TALK);
        types.push_back(EMOTE_ONESHOT_EXCLAMATION);
        types.push_back(EMOTE_ONESHOT_QUESTION);
        if (unit && (unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER) || unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER)))
        {
            types.push_back(EMOTE_ONESHOT_SALUTE);
        }
    }
    return types[urand(0, types.size() - 1)];
}
