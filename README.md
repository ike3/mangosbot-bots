# mangosbot-bots
Bot AI Core from ike3 for cmangos/mangos classic/tbc/wotlk

Want to build CMaNGOS + bots on your own? You can! (currently only on MS Windows)

💡  To build CMaNGOS + ike3 bots you should use a a fork of CMaNGOS with ike3 bots module
Vanilla - https://github.com/celguar/mangos-classic
TBC - https://github.com/celguar/mangos-tbc
WotLK - https://github.com/celguar/mangos-wotlk

Important: use --recursive and use branch "ike3-bots" when you clone any of these repos, e.g.:

git clone https://github.com/celguar/mangos-classic.git --recursive -b ike3-bots

💡  If you're new to building CMaNGOS, check the official Windows guide
https://github.com/cmangos/issues/wiki/Detailed-installation-guide-for-Microsoft-Windows

Important: to enable ike3 bots you need to check it in cmake ( BUILD_IKE3_BOTS ✅ )

💡  After successful build get aiplayerbot.conf file from "src/modules/Bots/playerbot/aiplayerbot%expansion.conf.dist" (based on expansion you use) and put it to the same folder where mangosd.conf and realmd.conf are, and remove ".dist" from its name

💡  DB modifications:
  a)  Go to "src/modules/Bots/sql"
  b)  Apply .sql files from "characters" folder to characters database
  c)  Apply .sql files from "world" folder to world database
  d)  Create a new database named "%expansion%playerbots", e.g. "classicplayerbots" for classic
  e)  Apply .sql files from "playerbot" folder to this new DB

After you complete all steps above you can check bots config and start your server. It'll take some time for the first time, as gear/characters for bots will be generated at first launch. Have fun! 🥳
