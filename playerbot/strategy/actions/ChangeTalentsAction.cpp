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
    int talents[4][8][3] = {};

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

        for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
        {
            uint32 spellId = talentInfo->RankID[rank];
            if (!spellId)
                continue;

            if(spellId>0 && talents[talentInfo->Col][talentInfo->Row][talentTabInfo->tabpage] == 0)
                talents[talentInfo->Col][talentInfo->Row][talentTabInfo->tabpage] = 1;

            if (bot->HasSpell(spellId))
            {
                talents[talentInfo->Col][talentInfo->Row][talentTabInfo->tabpage] = rank + 2;               
            }
        }
    }

    string sub;
    string line;
    string ret;
    int level = 9;
    int pointsTab = 0;
    int mostPoints = 0;
    int bestTab = 0;

    for (int tab = 0; tab < 3; tab++)
    {
 
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                if (talents[col][row][tab] == 0)
                    continue;

                pointsTab = pointsTab + talents[col][row][tab] - 1;

                level = level + talents[col][row][tab] - 1;

                sub = sub + to_string(talents[col][row][tab]-1);

                if (talents[col][row][tab] > 1)
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
        if (pointsTab > mostPoints)
        {
            bestTab = tab;
            mostPoints = pointsTab;
        }
        pointsTab = 0;
    }    

    out << "Talents for level: " << level << " main tree: " << bestTab << " spec: ";

    out << ret;

    ai->TellMaster(out);


	return false;
}
