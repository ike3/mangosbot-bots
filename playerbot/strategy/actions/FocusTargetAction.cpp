#include "botpch.h"
#include "../../playerbot.h"
#include "FocusTargetAction.h"

using namespace ai;

std::string LowercaseString(const std::string& string)
{
    std::string result = string;
    if (!string.empty())
    {
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }

    return result;
}

std::string LowercaseString(const char* string)
{
    std::string str = string;
    return LowercaseString(str);
}

bool FocusHealSetTargetAction::Execute(Event& event)
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
                // Look for the targets in the group
                const Group* group = bot->GetGroup();
                if (group)
                {
                    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
                    for (const std::string& targetName : targetNames)
                    {
                        ObjectGuid targetGuid;
                        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
                        {
                            Player* member = sObjectMgr.GetPlayer(itr->guid);
                            if (member)
                            {
                                const std::string memberName = LowercaseString(member->GetName());
                                if (memberName == targetName)
                                {
                                    targetGuid = itr->guid;
                                    break;
                                }
                            }
                        }

                        if (!targetGuid.IsEmpty())
                        {
                            focusHealTargets.push_back(targetGuid);
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