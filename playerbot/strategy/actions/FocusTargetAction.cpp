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
        const std::string targetName = LowercaseString(event.getParam());
        if (!targetName.empty())
        {
            ObjectGuid targetGuid;
            const bool removeTarget = targetName == "none" || targetName == "unset";
            if (!removeTarget)
            {
                // Look for the target in the group
                const Group* group = bot->GetGroup();
                if (group)
                {
                    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
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
                }
            }

            // Update the focus target value
            SET_AI_VALUE(ObjectGuid, "focus heal target", targetGuid);

            if (!targetGuid.IsEmpty())
            {
                // Set the focus heal target strategy if not set
                if (!ai->HasStrategy("focus heal target", BotState::BOT_STATE_COMBAT))
                {
                    ai->ChangeStrategy("+focus heal target", BotState::BOT_STATE_COMBAT);
                }

                std::stringstream message; message << "Set heal focus target to " << targetName;
                ai->TellPlayerNoFacing(requester, message.str());  
                return true;
            }
            else
            {
                if (removeTarget)
                {
                    ai->TellPlayerNoFacing(requester, "Removed heal focus target");
                }
                else
                {
                    std::stringstream message; message << "I'm not in a group with " << targetName;
                    ai->TellPlayerNoFacing(requester, message.str());
                }

                // Remove the focus heal target strategy if not set
                if (ai->HasStrategy("focus heal target", BotState::BOT_STATE_COMBAT))
                {
                    ai->ChangeStrategy("-focus heal target", BotState::BOT_STATE_COMBAT);
                }
            }
        }
        else
        {
            ai->TellPlayerNoFacing(requester, "Please provide a target name");
        }
    }
    else
    {
        ai->TellPlayerNoFacing(requester, "I'm not a healer or offhealer (please change my strats to heal or offheal)");
    }

    return false;   
}