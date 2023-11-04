#include "botpch.h"
#include "../../playerbot.h"
#include "ValueActions.h"

using namespace ai;

std::string LowercaseString(const std::string& string)
{
    std::string result = string;
    if (!string.empty())
    {
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) 
        { 
            return std::tolower(c); 
        });
    }

    return result;
}

std::string LowercaseString(const char* string)
{
    std::string str = string;
    return LowercaseString(str);
}

Player* FindGroupPlayerByName(Player* player, const std::string& playerName)
{
    Player* groupPlayer = nullptr;
    if (player)
    {
        const Group* group = player->GetGroup();
        if (group)
        {
            Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
            for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
            {
                Player* member = sObjectMgr.GetPlayer(itr->guid);
                if (member)
                {
                    const std::string memberName = LowercaseString(member->GetName());
                    if (memberName == playerName)
                    {
                        groupPlayer = member;
                    }
                }
            }
        }
    }

    return groupPlayer;
}

bool SetFocusHealTargetAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    if (ai->IsHeal(bot) || ai->HasStrategy("offheal", BotState::BOT_STATE_COMBAT))
    {
        const std::string param = LowercaseString(event.getParam());
        if (!param.empty())
        {
            bool removeTargets = false;
            std::vector<std::string> targetNames;

            // Multiple focus heal targets
            if (param.find(',') != std::string::npos)
            {
                std::string targetName;
                std::stringstream ss(param);

                while (std::getline(ss, targetName, ','))
                {
                    targetNames.push_back(targetName);
                }
            }
            else
            {
                removeTargets = param == "none" || param == "unset";
                if (!removeTargets)
                {
                    targetNames.push_back(param);
                }
            }

            std::list<ObjectGuid> focusHealTargets;
            if (removeTargets)
            {
                ai->TellPlayerNoFacing(requester, "Removed focus heal target");
            }
            else
            {
                if (bot->GetGroup())
                {
                    for (const std::string& targetName : targetNames)
                    {
                        Player* target = FindGroupPlayerByName(bot, targetName);
                        if (target)
                        {
                            focusHealTargets.push_back(target->GetObjectGuid());
                            std::stringstream message; message << "Added " << targetName << " to focus heal targets";
                            ai->TellPlayerNoFacing(requester, message.str());
                        }
                        else
                        {
                            std::stringstream message; message << "I'm not in a group with " << targetName;
                            ai->TellPlayerNoFacing(requester, message.str());
                        }
                    }
                }
                else
                {
                    ai->TellPlayerNoFacing(requester, "I'm not in a group");
                }
            }

            SET_AI_VALUE(std::list<ObjectGuid>, "focus heal target", focusHealTargets);

            if (focusHealTargets.empty())
            {
                // Remove the focus heal target strategy if not set
                if (ai->HasStrategy("focus heal target", BotState::BOT_STATE_COMBAT))
                {
                    ai->ChangeStrategy("-focus heal target", BotState::BOT_STATE_COMBAT);
                }

                return removeTargets;
            }
            else
            {
                // Set the focus heal target strategy if not set
                if (!ai->HasStrategy("focus heal target", BotState::BOT_STATE_COMBAT))
                {
                    ai->ChangeStrategy("+focus heal target", BotState::BOT_STATE_COMBAT);
                }

                return true;
            }
        }
        else
        {
            ai->TellPlayerNoFacing(requester, "Please provide one or more player names");
        }
    }
    else
    {
        ai->TellPlayerNoFacing(requester, "I'm not a healer or offhealer (please change my strats to heal or offheal)");
    }

    return false;
}

bool SetWaitForAttackTimeAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string newTimeStr = event.getParam();
    if (!newTimeStr.empty())
    {
        // Check if the param is a number
        if (newTimeStr.find_first_not_of("0123456789") == std::string::npos)
        {
            const int newTime = stoi(newTimeStr.c_str());
            if (newTime <= 99)
            {
                ai->GetAiObjectContext()->GetValue<uint8>("wait for attack time")->Set(newTime);
                ostringstream out; out << "Wait for attack time set to " << newTime << " seconds";
                ai->TellPlayerNoFacing(requester, out);
                return true;
            }
            else
            {
                ai->TellPlayerNoFacing(requester, "Please provide valid time to set (in seconds) between 1 and 99");
            }
        }
        else
        {
            ai->TellPlayerNoFacing(requester, "Please provide valid time to set (in seconds) between 1 and 99");
        }
    }
    else
    {
        ai->TellPlayerNoFacing(requester, "Please provide a time to set (in seconds)");
    }

    return false;
}

bool SetFollowTargetAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    const std::string param = LowercaseString(event.getParam());
    if (!param.empty())
    {
        const bool removeTarget = param == "none" || param == "unset";
        if (removeTarget)
        {
            SET_AI_VALUE(Unit*, "manual follow target", nullptr);
            ai->TellPlayerNoFacing(requester, "Removed follow target");
            return true;
        }
        else
        {
            if (bot->GetGroup())
            {
                Player* target = FindGroupPlayerByName(bot, param);
                if (target)
                {
                    if (target != bot)
                    {
                        SET_AI_VALUE(Unit*, "manual follow target", target);
                        std::stringstream message; message << "Set " << param << " as the follow target";
                        ai->TellPlayerNoFacing(requester, message.str());
                        return true;
                    }
                    else
                    {
                        ai->TellPlayerNoFacing(requester, "I can't follow myself!");
                    }
                }
                else
                {
                    std::stringstream message; message << "I'm not in a group with " << param;
                    ai->TellPlayerNoFacing(requester, message.str());
                }
            }
            else
            {
                ai->TellPlayerNoFacing(requester, "I'm not in a group");
            }
        }
    }
    else
    {
        ai->TellPlayerNoFacing(requester, "Please provide one or more player names");
    }

    return false;
}
