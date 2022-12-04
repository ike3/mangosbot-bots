#pragma once

using namespace std;

typedef set<uint32> ItemIds;
typedef set<uint32> SpellIds;

namespace ai
{
    class ChatHelper : public PlayerbotAIAware
    {
    public:
        ChatHelper(PlayerbotAI* ai);

    public:
        static string formatMoney(uint32 copper);
        static uint32 parseMoney(string& text);

        static string formatQuest(Quest const* quest);

        static string formatItem(ItemPrototype const * proto, int count = 0, int total = 0);
        static string formatQItem(uint32 itemId);
        static ItemIds parseItems(string& text);
        static uint32 parseItemQuality(string text);
        static bool parseItemClass(string text, uint32* itemClass, uint32* itemSubClass);
        static uint32 parseSlot(string text);

        static string formatSpell(SpellEntry const *sInfo);
        uint32 parseSpell(string& text);

        static string formatGameobject(GameObject* go);
        static list<ObjectGuid> parseGameobjects(string& text);

        static string formatWorldobject(WorldObject* wo);

        static string formatWorldEntry(int32 entry);
        static list<int32> parseWorldEntries(string& text);

        static string formatQuestObjective(string name, int available, int required);

        static string formatValue(string type, string code, string name, string color = "0000FFFF");
        static string parseValue(string type, string& text);

        static string formatChat(ChatMsg chat);
        static ChatMsg parseChat(string& text);

        static string formatClass(Player* player, int spec);
        static string formatClass(uint8 cls);

        static string formatRace(uint8 race);

        static string formatSkill(uint32 skill);
        uint32 parseSkill(string& text);

        static string formatBoolean(bool flag);       
       
        static bool parseable(string text);

        void eraseAllSubStr(std::string& mainStr, const std::string& toErase);

    private:
        static map<string, uint32> consumableSubClasses;
        static map<string, uint32> tradeSubClasses;
        static map<string, uint32> itemQualities;
        static map<string, uint32> projectileSubClasses;
        static map<string, uint32> slots;
        static map<string, uint32> skills;
        static map<string, ChatMsg> chats;
        static map<uint8, string> classes;
        static map<uint8, string> races;
        static map<uint8, map<uint8, string> > specs;
    };
};
