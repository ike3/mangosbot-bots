DROP TABLE IF EXISTS `ai_playerbot_rpg_races`;

CREATE TABLE `ai_playerbot_rpg_races` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `entry` bigint(20),
  `race` bigint(20),
  `minl` bigint(20),
  `maxl` bigint(20),
  PRIMARY KEY (`id`),
  KEY `entry` (`entry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DELETE FROM `ai_playerbot_rpg_races`;

-- say

INSERT INTO `ai_playerbot_rpg_races` VALUES
--
--       DRAENEI
--
-- Draenei Azumeryst Isle
(NULL, 16553, 11, 1, 10),
-- Draenei Bloodmyst Isle
(NULL, 17553, 11, 10, 20),
-- Draenei Exodar
(NULL, 16739, 11, 1, 80),
--
--       BLOOD ELVES
--
-- Blood Elves Eversong Woods
(NULL, 15397, 10, 1, 10),
-- Blood Elves Ghostlands
(NULL, 16542, 10, 10, 20),
-- Blood Elves Silvermoon
(NULL, 16618, 10, 1, 80),
--
--       HUMANS
--
-- Innkeeper Farley, Goldshire
(NULL, 295, 1, 1, 15),
-- Innkeeper Heather, Westfall
(NULL, 8931, 1, 10, 20),
--
--       DWARVES & GNOMES
--
-- Innkeeper Belm, Kharanos
(NULL, 1247, 3, 1, 15),
(NULL, 1247, 7, 1, 15),
-- Innkeeper Hearthstove, Loch Modan
(NULL, 6734, 3, 10, 20),
(NULL, 6734, 7, 10, 20),
--
--       NIGHT ELVES
--
-- Innkeeper Keldamyr, Dolanaar
(NULL, 6736, 4, 1, 15),
-- Innkeeper Shaussiy, Auberdine
(NULL, 6737, 4, 10, 20),
--
--       ALLIANCE CITIES
--
-- Innkeeper Saelienne, Darnassus
(NULL, 6735, 4, 10, 80), -- elves
--
-- Innkeeper Firebrew, Ironforge
(NULL, 5111, 3, 10, 80), -- dwarves
(NULL, 5111, 7, 10, 80), -- gnomes
(NULL, 5111, 4, 10, 80), -- elves
(NULL, 5111, 1, 10, 80), -- human
--
-- Innkeeper Allison, Stormwind
(NULL, 6740, 1, 10, 80), -- human
(NULL, 6740, 3, 10, 80), -- dwarves	
(NULL, 6740, 4, 10, 80), -- elves	
(NULL, 6740, 7, 10, 80), -- gnomes
--
--       ALLIANCE CONTESTED LOCATIONS
--
-- Innkeeper Kimlya, Astranaar
--
(NULL, 6738, 4, 15, 30), -- elves
--
-- Innkeeper Faralia, Stonetalon Peak
--
(NULL, 16458, 4, 15, 27), -- elves
--
-- Innkeeper Lyshaerya, Desolase
--
(NULL, 11103, 4, 30, 40), -- elves
--
-- Innkeeper Shyria, Feathermoon, Feralas
(NULL, 7736, 4, 40, 50), -- elves
--
-- Falfindel Waywarder, Feralas elf camp
(NULL, 4048, 4, 40, 50), -- elves
--
-- Innkeeper Helbrek, Wetlands
--
(NULL, 1464, 3, 20, 30), -- dwarves
(NULL, 1464, 7, 20, 30), -- gnomes
--
-- Innkeeper Trelayne, Duskwood
--
(NULL, 6790, 1, 18, 30), -- human
(NULL, 6790, 3, 18, 30), -- dwarves
(NULL, 6790, 7, 18, 30), -- gnomes
--
-- Innkeeper Brianna, Redridge Mountains
--
(NULL, 6727, 1, 15, 25), -- human
--
-- Innkeeper Anderson, Southshore, Hillsbrad
(NULL, 2352, 1, 20, 30), -- human
(NULL, 2352, 3, 20, 30), -- dwarves
(NULL, 2352, 7, 20, 30), -- gnomes
--
-- Captain Nials, Refuge Pointe, Arathi
(NULL, 2700, 1, 30, 40), -- human
(NULL, 2700, 3, 30, 40), -- dwarves
(NULL, 2700, 7, 30, 40), -- gnomes
--
-- Lt. Doren, Stranglethorn Vale
(NULL, 469, 1, 30, 45), -- human
(NULL, 469, 3, 30, 45), -- dwarves
(NULL, 469, 4, 30, 45), -- elves
(NULL, 469, 7, 30, 45), -- gnomes
--
-- Innkeeper Janene, Theramore
(NULL, 6272, 1, 35, 45), -- human
(NULL, 6272, 3, 35, 45), -- dwarves
(NULL, 6272, 7, 35, 45), -- gnomes
--
-- Innkeeper Prospector Ryedol, Badlands Q-giver
(NULL, 2910, 3, 35, 45), -- dwarves
(NULL, 2910, 7, 35, 45), -- gnomes
--
-- Innkeeper Thulfram, Hinterlands, Dwarven Outpost
(NULL, 7744, 3, 40, 50), -- dwarves
(NULL, 7744, 1, 40, 50), -- human
(NULL, 7744, 7, 40, 50), -- gnomes
--
-- Loh'atu, Azshara alliance camp Q-giver 11548
(NULL, 11548, 4, 45, 55), -- elves
(NULL, 11548, 1, 45, 55), -- human
--
-- Thadius Grimshade, Nethergarde Keep, Blasted Lands
(NULL, 8022, 1, 45, 55), -- human
(NULL, 8022, 3, 45, 55), -- dwarves
(NULL, 8022, 4, 45, 55), -- elves
(NULL, 8022, 7, 45, 55), -- gnomes
--
-- Gothine the Hooded, Felwood Alliance camp
(NULL, 9465, 4, 48, 55), -- elves
(NULL, 9465, 1, 48, 55), -- human
(NULL, 9465, 3, 48, 55), -- dwarves
(NULL, 9465, 7, 48, 55), -- gnomes
--
-- Muigin, Alliance Q-giver, Un'Goro
(NULL, 9119, 1, 48, 55), -- human
(NULL, 9119, 3, 48, 55), -- dwarves
(NULL, 9119, 4, 48, 55), -- elves
(NULL, 9119, 7, 48, 55), -- gnomes
--
-- Alchemist Arbington, West Plaguelands, Human
(NULL, 11056, 1, 51, 58), -- human
(NULL, 11056, 3, 51, 58), -- dwarves
(NULL, 11056, 4, 51, 58), -- elves
(NULL, 11056, 7, 51, 58), -- gnomes
--
-- Borgus Stourarm, Alliance Taxi, Burning Steppes
(NULL, 2299, 1, 50, 60), -- human
(NULL, 2299, 3, 50, 60), -- dwarves
(NULL, 2299, 4, 50, 60), -- elves
(NULL, 2299, 7, 50, 60), -- gnomes
--
-- Marshal Bluewall, Alliance camp, Silithus
(NULL, 17080, 1, 55, 60), -- human
(NULL, 17080, 3, 55, 60), -- dwarves
(NULL, 17080, 4, 55, 60), -- elves
(NULL, 17080, 7, 55, 60), -- gnomes
--
--
--       ALLIANCE MOUNT VENDORS
--
-- Milli Featherwhistle, Gnome Mechanostrider merchant, Dun Morogh
-- (NULL, 7955, 3),
-- (NULL, 7955, 7),
--
-- Lelanai, Night Elf Night Saber vendor, Darnassus
-- (NULL, 4730, 4),
--
-- Katie Hunter, Human Horse vendor, Elwynn Forest
-- (NULL, 384, 1),
--
--       HORDE
--
--       ORCS & TROLLS
--
-- Innkeeper Grosk, Durotar
(NULL, 6928, 2, 1, 10),
(NULL, 6928, 8, 1, 10),
--
--       TAUREN
--
-- Innkeeper Pala, Mulgore
(NULL, 6746, 6, 1, 10),
--
--       UNDEAD
--
-- Innkeeper Renee, Brill, Tirisfal Glades
(NULL, 5688, 5, 1, 15),
-- Innkeeper Bates, The Sepulcher, Silverpine Forest
(NULL, 6739, 5, 10, 20),
--
--       HORDE CITIES
--
-- Innkeeper Gryshka, Orgrimmar
(NULL, 6929, 2, 10, 80), -- orcs
(NULL, 6929, 8, 10, 80), -- trolls
(NULL, 6929, 6, 10, 80), -- tauren
(NULL, 6929, 5, 10, 80), -- undead
--
-- Innkeeper Pala, Thunder Bluff, Mulgore
(NULL, 6746, 6, 10, 80), -- tauren
--
-- Innkeeper Norman, Undercity
(NULL, 6741, 5, 10, 80), -- undead
--
--       HORDE CONTESTED LOCATIONS
--
-- Innkeeper Boorand Plainswind, Crossroads, Barrens
(NULL, 3934, 2, 10, 25), -- orcs
(NULL, 3934, 6, 10, 25), -- tauren
(NULL, 3934, 8, 10, 25), -- trolls
--
-- Innkeeper Byula, Camp Taurajo, Barrens
(NULL, 7714, 2, 10, 25), -- orcs
(NULL, 7714, 6, 10, 25), -- tauren
(NULL, 7714, 8, 10, 25), -- trolls
--
-- Innkeeper Jayka, Stonetalon, Red Rock Retreat
(NULL, 7731, 2, 15, 27), -- orcs
(NULL, 7731, 6, 15, 27), -- tauren
(NULL, 7731, 8, 15, 27), -- trolls
--
-- Innkeeper Abeqwa, Thousand Needles
(NULL, 11116, 2, 25, 35), -- orcs
(NULL, 11116, 6, 25, 35), -- tauren
(NULL, 11116, 8, 25, 35), -- trolls
--
-- Innkeeper Shay, Tarren Mill, Hillsbrad
(NULL, 2388, 5, 20, 30), -- undead
--
-- Innkeeper Greul, Feralas, Horde
(NULL, 7737, 6, 40, 50), -- tauren
--
-- Innkeeper Kaylisk, Splitertree, Ashenvale
(NULL, 12196, 2, 18, 30), -- orcs
(NULL, 12196, 8, 18, 30), -- trolls
--
-- Marukai, Zoram'gar, Ashenvale
(NULL, 12719, 2, 18, 30), -- orcs
(NULL, 12719, 8, 18, 30), -- trolls
--
-- Innkeeper Sikewa, Desolace
(NULL, 11106, 2, 30, 40), -- orcs
(NULL, 11106, 6, 30, 40), -- tauren
(NULL, 11106, 8, 30, 40), -- trolls
--
-- Innkeeper Adegwa, Arathi, Hammerfall
(NULL, 9501, 2, 30, 40), -- orcs
(NULL, 9501, 5, 30, 40), -- undead
(NULL, 9501, 6, 30, 40), -- tauren
(NULL, 9501, 8, 30, 40), -- trolls
--
-- Innkeeper Lard, Revantusk Village , Hinterlands
(NULL, 14731, 2, 40, 50), -- orcs
(NULL, 14731, 5, 40, 50), -- undead
(NULL, 14731, 6, 40, 50), -- tauren
(NULL, 14731, 8, 40, 50), -- trolls
--
-- Innkeeper Shul'kar, Kargath Outpost, Badlands
(NULL, 9356, 2, 35, 45), -- orcs
(NULL, 9356, 5, 35, 45), -- undead
(NULL, 9356, 6, 35, 45), -- tauren
(NULL, 9356, 8, 35, 45), -- trolls
--
-- Innkeeper Karakul, Swamp of Sorrows
(NULL, 6930, 2, 35, 45), -- orcs
(NULL, 6930, 5, 35, 45), -- undead
(NULL, 6930, 6, 35, 45), -- tauren
(NULL, 6930, 8, 35, 45), -- trolls
--
-- Innkeeper Thulbek, Grom Gol, Stranglethorn Vale
(NULL, 5814, 2, 30, 45), -- orcs
(NULL, 5814, 5, 30, 45), -- undead
(NULL, 5814, 6, 30, 45), -- tauren
(NULL, 5814, 8, 30, 45), -- trolls
--
-- Overlord Mok'Morokk, Dustwallow Marsh
(NULL, 4500, 2, 35, 45), -- orcs
(NULL, 4500, 6, 35, 45), -- tauren
(NULL, 4500, 8, 35, 45), -- trolls
--
-- Jediga, Azshara horde camp
(NULL, 8587, 2, 45, 55), -- orcs
(NULL, 8587, 8, 45, 55), -- trolls
(NULL, 8587, 6, 45, 55), -- tauren
--
-- Winna Hazzard, Felwood horde camp
(NULL, 9996, 2, 48, 55), -- orcs
(NULL, 9996, 6, 48, 55), -- tauren
(NULL, 9996, 8, 48, 55), -- trolls
(NULL, 9996, 5, 48, 55), -- undead
--
-- Larion, Horde Q-giver, Un'Goro
(NULL, 9118, 2, 48, 55), -- orcs
(NULL, 9118, 6, 48, 55), -- tauren
(NULL, 9118, 8, 48, 55), -- trolls
(NULL, 9118, 5, 48, 55), -- undead
--
-- Vahgruk, Horde Taxi, Burning Steppes
(NULL, 13177, 2, 50, 60), -- orcs
(NULL, 13177, 5, 50, 60), -- undead
(NULL, 13177, 6, 50, 60), -- tauren
(NULL, 13177, 8, 50, 60), -- trolls
--
-- General Kirika, Horde camp, Silithus
(NULL, 17079, 2, 55, 60), -- orcs
(NULL, 17079, 5, 55, 60), -- undead
(NULL, 17079, 6, 55, 60), -- tauren
(NULL, 17079, 8, 55, 60), -- trolls
--
--        NEUTRAL AREAS
--
-- Innkeeper Skindle, Booty Bay 6807 (Neutral)
(NULL, 6807, 0, 30, 45),
-- Innkeeper Wiley, Ratchet 6791 (Neutral)
(NULL, 6791, 2, 10, 25),
(NULL, 6791, 8, 10, 25),
-- Innkeeper Fizzgrimble, Tanaris 7733 (Neutral)
(NULL, 7733, 0, 40, 50),
-- Master Smith Burninate, Searing Gorge
(NULL, 14624, 0, 45, 50),
-- Marin Noggenfogger 7564 (Neutral)
-- Innkeeper Vizzie, Everlook 11118 (Neutral)
(NULL, 11118, 0, 53, 60),
-- Calandrath, Silithus 15174 (Neutral)
(NULL, 15174, 0, 55, 60),
-- Jessica Chambers, East Plaguelands 16256 (Neutral)
(NULL, 16256, 0, 53, 60),
--
--
--
--       UNUSED
--
--
-- Bashana Runetotem, Thunder Bluff (Tauren npc in TB)
-- (NULL, 9087, 6),
--
-- Alchemist Arbington, West Plaguelands, Human
-- (NULL, 11056, 1),
-- (NULL, 11056, 3),
-- (NULL, 11056, 4),
-- (NULL, 11056, 7),
--
-- Lokhtos Darkbargainer, Blackrock Depths, 12944
--
-- Gregan Brewspewer, Feralas, Dwarf (Some swarf Q-giver in Feralas)
-- (NULL, 7775, 3),
--
-- Augustus the Touched, East Plaguelands, Undead (Some undead vendor near stratholme)
-- (NULL, 12384, 5),
--
-- ENDING PLACEHOLDER
(NULL, 6807, 15, 90, 90)
;
