#include "botpch.h"
#include "../../playerbot.h"
#include "ChangeStrategyAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool ChangeCombatStrategyAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string text = event.getParam();
    text = text.empty() ? getName() : text;

    ai->ChangeStrategy(text, BotState::BOT_STATE_COMBAT);
    if (event.getSource() == "co")
    {
        vector<string> splitted = split(text, ',');
        for (vector<string>::iterator i = splitted.begin(); i != splitted.end(); i++)
        {
            const char* name = i->c_str();
            switch (name[0])
            {
                case '+':
                case '-':
                case '~':
                    sPlayerbotDbStore.Save(ai);
                    break;
            }
        }
    }

    if (text.find("?") != std::string::npos)
    {
        ai->PrintStrategies(requester, BotState::BOT_STATE_COMBAT);
    }
    
    return true;
}

bool ChangeNonCombatStrategyAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string text = event.getParam();
    text = text.empty() ? getName() : text;

    ai->ChangeStrategy(text, BotState::BOT_STATE_NON_COMBAT);
    if (event.getSource() == "nc")
    {
        vector<string> splitted = split(text, ',');
        for (vector<string>::iterator i = splitted.begin(); i != splitted.end(); i++)
        {
            const char* name = i->c_str();
            switch (name[0])
            {
                case '+':
                case '-':
                case '~':
                    sPlayerbotDbStore.Save(ai);
                    break;
            }
        }
    }

    if (text.find("?") != std::string::npos)
    {
        ai->PrintStrategies(requester, BotState::BOT_STATE_NON_COMBAT);
    }

    return true;
}

bool ChangeDeadStrategyAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string text = event.getParam();
    text = text.empty() ? getName() : text;

    ai->ChangeStrategy(text, BotState::BOT_STATE_DEAD);

    if (text.find("?") != std::string::npos)
    {
        ai->PrintStrategies(requester, BotState::BOT_STATE_DEAD);
    }

    return true;
}

bool ChangeReactionStrategyAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string text = event.getParam();
    text = text.empty() ? getName() : text;

    ai->ChangeStrategy(text, BotState::BOT_STATE_REACTION);

    if (text.find("?") != std::string::npos)
    {
        ai->PrintStrategies(requester, BotState::BOT_STATE_REACTION);
    }

    return true;
}

bool ChangeAllStrategyAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string text = event.getParam();
    string strategyName = text.empty() ? strategy : text;

    uint32 account = sObjectMgr.GetPlayerAccountIdByGUID(bot->GetObjectGuid());
    if (sPlayerbotAIConfig.IsInRandomAccountList(account) && ai->GetMaster() && ai->GetMaster()->GetSession()->GetSecurity() < SEC_GAMEMASTER)
    {
        if (strategyName.find("loot") != string::npos || strategyName.find("gather") != string::npos)
        {
            ai->TellError(requester, "You can change any strategy except loot and gather");
            return false;
        }
    }

    ai->ChangeStrategy(strategyName, BotState::BOT_STATE_ALL);

    if (event.getSource() == "nc" || event.getSource() == "co")
    {
        vector<string> splitted = split(text, ',');
        for (vector<string>::iterator i = splitted.begin(); i != splitted.end(); i++)
        {
            const char* name = i->c_str();
            switch (name[0])
            {
                case '+':
                case '-':
                case '~':
                    sPlayerbotDbStore.Save(ai);
                    break;
            }
        }
    }

    if (text.find("?") != std::string::npos)
    {
        ai->PrintStrategies(requester, BotState::BOT_STATE_ALL);
    }

    return true;
}