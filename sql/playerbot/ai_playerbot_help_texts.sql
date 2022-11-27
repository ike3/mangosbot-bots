-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               5.7.26 - MySQL Community Server (GPL)
-- Server OS:                    Win32
-- HeidiSQL Version:             12.2.0.6576
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

-- Dumping structure for table classicplayerbots.ai_playerbot_help_texts
DROP TABLE IF EXISTS `ai_playerbot_help_texts`;
CREATE TABLE IF NOT EXISTS `ai_playerbot_help_texts` (
  `id` smallint(20) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL COMMENT 'name - type:subject',
  `template_changed` tinyint(4) NOT NULL DEFAULT '0' COMMENT 'template_changed - Has the template text changed after text?',
  `template_text` text NOT NULL COMMENT 'generated text',
  `text` text NOT NULL COMMENT 'text',
  `text_loc1` text NOT NULL,
  `text_loc2` text NOT NULL,
  `text_loc3` text NOT NULL,
  `text_loc4` text NOT NULL,
  `text_loc5` text NOT NULL,
  `text_loc6` text NOT NULL,
  `text_loc7` text NOT NULL,
  `text_loc8` text NOT NULL,
  `locs_updated` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'locs_updated - Have the loc texts been updated?',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- Dumping data for table classicplayerbots.ai_playerbot_help_texts: ~11 rows (approximately)
INSERT INTO `ai_playerbot_help_texts` (`id`, `name`, `template_changed`, `template_text`, `text`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`, `locs_updated`) VALUES
	(1, 'help:main', 0, '', '----This is the main help page----\r\nPlease copy a link to get more information about a topic.\r\nObjects: [h:object:strategy|strategies][h:object:triggers|trigger][h:object:actions|action][h:object:value|values][h:object:chatfilter|chatfilters]', '', '', '', '', '', '', '', '', 0),
	(2, 'object:strategy', 0, '', 'Strategies are rules that controle bot behavior. They combine [h:object:trigger|triggers] and [h:object:action|actions].\r\nStrategies: [h:strategy|grind][h:strategy|rpg][h:strategy|travel]', '', '', '', '', '', '', '', '', 0),
	(3, 'object:trigger', 0, '', 'Triggers are conditions are evalated by the bot which helps it decide what to do. \r\nWhen a [h:object|strategy] combines a trigger with [h:object:action|actions] it will result in bot behavior. \r\nStrategies: [h:strategy|grind][h:strategy|rpg][h:strategy|travel]', '', '', '', '', '', '', '', '', 0),
	(4, 'object:action', 0, '', 'Actions are specific interactions the bot can have with the world or itself. \r\nUsually the result of a [h:object:trigger] enabled by a [h:object|strategy]. \r\nActions: [h:action|choose rpg target][h:action|move to rpg target][h:action|rpg]', '', '', '', '', '', '', '', '', 0),
	(5, 'object:value', 0, '', 'Values are pieces of information the bot has about itself or the world.\r\nThey are used in [h:object:trigger|triggers] and [h:object:action|actions] for evaluation purposes\r\nValues: [h:value|rpg target][h:value|near rpg target]', '', '', '', '', '', '', '', '', 0),
	(6, 'strategy:rpg', 0, '', 'Rpg [h:object|strategy] [toggle][enable][disable]\r\nThis strategy makes bot move between npcs to automatically do various interaction.\r\n\r\nThis is the main rpg strategy which make bots pick and move to various rpg targets. \r\nThe interactions included in this strategy are limited to emotes and wait.\r\n\r\nOther rpg strategies: [h:strategy|rpg quest][h:strategy|rpg vendor]...\r\n\r\nBehavior:\r\nOn [h:trigger|no rpg target] do [h:action|choose rpg target] (4)\r\nOn [h:trigger|far from rpg target] do [h:action|move to rpg target] (3)', '', '', '', '', '', '', '', '', 0),
	(7, 'trigger:far from rpg target', 0, '', 'Far from rpg target [h:object|trigger]\r\nThis trigger activates when the bot is 5y away from it\'s current rpg target.\r\n\r\nBehavior:\r\nThis trigger activates [h:action|move to travel target] (4) in [h:strategy|rpg strategy]\r\n\r\nValues used:\r\n[h:value|rpg target]', '', '', '', '', '', '', '', '', 0),
	(8, 'action:move to travel target', 0, '', 'Move to travel target [h:object|action] [execute]\r\nThis will make the bot move towards the current rpg target.\r\n\r\nBehavior:\r\nThis activates on [h:trigger|far from travel target] (4) in [h:strategy|rpg strategy]\r\n\r\nValues used:\r\n[h:value|rpg target]', '', '', '', '', '', '', '', '', 0),
	(9, 'value:rpg target', 0, '', 'Rpg target [h:object|value] [current value]\r\nThis value contains the [objectGuid] of a [unit] or [gameObject] to move to and rpg with. This value is manually set.\r\n\r\nSet in: [h:action|choose rpg target]\r\nUsed in: [h:trigger|far from rpg target][h:action|rpg]', '', '', '', '', '', '', '', '', 0),
	(10, 'object:chatfilter', 0, '', 'Chatfilters is a bot selection mechanism that can be precede a chat command to only reach specific bots.\r\nExample "@<filter> who" will only make bots who fit the filter respond.\r\nMultiple filtes can be used at the same time.\r\n\r\nChat filters: [h:chatfilter|guild]', '', '', '', '', '', '', '', '', 0),
	(11, 'chatfilter:guild', 0, '', 'Guild [h:object|chatfilter]\r\nUsage: @guild, @guild=<guildname> \r\nOnly select bots in a (specific guild)\r\n\r\nExample: [@guild who]', '', '', '', '', '', '', '', '', 0);

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
