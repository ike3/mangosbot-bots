#include "botpch.h"
#include "../../playerbot.h"
#include "CheatAction.h"


using namespace ai;

bool CheatAction::Execute(Event event)
{
    string param = event.getParam();

    uint32 cheatMask = (uint32)ai->GetCheat();

    vector<string> splitted = split(param, ',');
    for (vector<string>::iterator i = splitted.begin(); i != splitted.end(); i++)
    {
        const char* name = i->c_str();
        switch (name[0])
        {
        case '+':
            cheatMask |= (uint32)GetCheatMask(name + 1);
            break;
        case '-':
            cheatMask ^= (uint32)GetCheatMask(name + 1);
            break;
        case '?':
            ListCheats();
            return true;
        }
    }

    ai->SetCheat(BotCheatMask(cheatMask));

    return true;
}

BotCheatMask CheatAction::GetCheatMask(string cheat)
{
    if (cheat=="taxi")
        return BotCheatMask::taxi;
    if (cheat == "gold")
        return BotCheatMask::gold;
    if (cheat == "health")
        return BotCheatMask::health;
    if (cheat == "mana")
        return BotCheatMask::mana;
    if (cheat == "power")
        return BotCheatMask::power;

    return BotCheatMask::none;
}

string CheatAction::GetCheatName(BotCheatMask cheatMask)
{
    switch (cheatMask)
    {
    case BotCheatMask::taxi:
            return "taxi";
    case BotCheatMask::gold:
        return "gold";
    case BotCheatMask::health:
        return "health";
    case BotCheatMask::mana:
        return "mana";
    case BotCheatMask::power:
        return "power";
    default: 
        return "none";
    }
}

void CheatAction::ListCheats()
{
    ostringstream out;
    for (int i = 0; i < log2((uint32)BotCheatMask::maxMask); i++)
    {
        BotCheatMask cheatMask = BotCheatMask(1 << i);
       if ((uint32)cheatMask & (uint32)sPlayerbotAIConfig.botCheatMask)
           out << "[conf:" << GetCheatName(BotCheatMask(cheatMask)) << "]";
       else if (ai->HasCheat(cheatMask))
           out << "[" << GetCheatName(BotCheatMask(cheatMask)) << "]";
    }

    ai->TellMasterNoFacing(out);
}