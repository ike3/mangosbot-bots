#include "botpch.h"
#include "../../playerbot.h"
#include "HelpAction.h"
#include "ChatActionContext.h"
#include "PlayerbotHelpMgr.h"

using namespace ai;

HelpAction::HelpAction(PlayerbotAI* ai) : Action(ai, "help")
{
    chatContext = new ChatActionContext();
}

HelpAction::~HelpAction()
{
    delete chatContext;
}

bool HelpAction::Execute(Event& event)
{
    string param = event.getParam();
    string helpTopic;
    if(param.find("Hvalue:help") != string::npos)
        helpTopic = ChatHelper::parseValue("help",param);
    else if (param.find("[h:") != string::npos)
    {
        helpTopic = param.substr(3,param.size()-4);
    }

    if (helpTopic.empty())
        helpTopic = "help:main";


    ostringstream out;
    string helpTekst = sPlayerbotHelpMgr.GetBotText(helpTopic);

    if (!helpTekst.empty())
    {
        vector<string> lines = Qualified::getMultiQualifiers(helpTekst, "\n");

        for (auto& line : lines)
        {
            ai->TellMasterNoFacing(line, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, true, false);
        }

        return true;


        TellChatCommands();
        TellStrategies();
    }
    return true;
}

void HelpAction::TellChatCommands()
{
    ostringstream out;
    out << "Whisper any of: ";
    out << CombineSupported(chatContext->supports());
    out << ", [item], [quest] or [object] link";
    ai->TellError(out.str());
}

void HelpAction::TellStrategies()
{
    ostringstream out;
    out << "Possible strategies (co/nc/dead commands): ";
    out << CombineSupported(ai->GetAiObjectContext()->GetSupportedStrategies());
    ai->TellError(out.str());
}

string HelpAction::CombineSupported(set<string> commands)
{
    ostringstream out;

    for (set<string>::iterator i = commands.begin(); i != commands.end(); )
	{
        out << *i;
		if (++i != commands.end())
			out << ", ";
	}

    return out.str();
}
