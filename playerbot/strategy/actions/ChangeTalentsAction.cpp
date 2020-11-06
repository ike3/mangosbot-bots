#include "botpch.h"
#include "../../playerbot.h"
#include "ChangeTalentsAction.h"

using namespace ai;

bool ChangeTalentsAction::Execute(Event event)
{
	string param = event.getParam();

	ostringstream out;

    uint32 classMask = bot->getClassMask();
    uint32 lastTab = -1;
    int talents[4][8][3] = { 80 };

    map<uint32, vector<TalentEntry const*> > spells;
    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if ((classMask & talentTabInfo->ClassMask) == 0)
            continue;

        talents[talentInfo->Col][talentInfo->Row][talentTabInfo->tabpage] = 80;

        for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
        {
            uint32 spellId = talentInfo->RankID[rank];
            if (!spellId)
                continue;

            if(spellId>0)
                talents[talentInfo->Col][talentInfo->Row][talentTabInfo->tabpage] = 0;


            if (bot->HasSpell(spellId))
                talents[talentInfo->Col][talentInfo->Row][talentTabInfo->tabpage] = rank + 1;
        }
    }

    string sub;
    string line;
    string ret;

    for (int tab = 0; tab < 3; tab++)
    {
 
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                if (talents[col][row][tab] == 80)
                    continue;

                sub = sub + to_string(talents[col][row][tab]);

                if (talents[col][row][tab] > 0)
                {
                    line = line + sub;
                    sub = "";
                }
            }
        }
        sub = "";
        if (line.empty())
            line = "0";
        if (tab > 0)
            ret = ret + "-";
        ret = ret + line;
        line = "";
    }

    out << ret;

    ai->TellMaster(out);

	return false;
}
