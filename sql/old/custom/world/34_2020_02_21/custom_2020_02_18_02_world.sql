-- Pathing for  Entry: 840 'TDB FORMAT' 
SET @NPC := 317795;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`= 0, `MovementType`= 2, `position_x`= -10302.25, `position_y`= -1146.264, `position_z`= 22.72622 WHERE `guid`= @NPC;
DELETE FROM `creature_addon` WHERE `guid`= @NPC;
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (@NPC, @PATH, 0, 0, 1, 0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES
(@PATH, 1, -10302.25, -1146.264, 22.72622, 0, 106, 0, 0, 100, 0),
(@PATH, 2, -10320.51, -1125.687, 21.68385, 0, 0, 0, 0, 100, 0),
(@PATH, 3, -10351.73, -1116.864, 21.51039, 0, 0, 0, 0, 100, 0),
(@PATH, 4, -10391.54, -1118.014, 22.62218, 0, 0, 0, 0, 100, 0),
(@PATH, 5, -10416.29, -1138.326, 24.56924, 0, 0, 0, 0, 100, 0),
(@PATH, 6, -10434.21, -1166.358, 28.01633, 0, 0, 0, 0, 100, 0),
(@PATH, 7, -10455.65, -1178.642, 27.41125, 0, 0, 0, 0, 100, 0),
(@PATH, 8, -10483.37, -1182.986, 27.85222, 0, 0, 0, 0, 100, 0),
(@PATH, 9, -10512.17, -1186.835, 28.15375, 0, 396, 0, 0, 100, 0),
(@PATH, 10, -10483.37, -1182.986, 27.85222, 0, 0, 0, 0, 100, 0),
(@PATH, 11, -10455.65, -1178.642, 27.41125, 0, 0, 0, 0, 100, 0),
(@PATH, 12, -10434.21, -1166.358, 28.01633, 0, 0, 0, 0, 100, 0),
(@PATH, 13, -10416.36, -1138.391, 24.6398, 0, 0, 0, 0, 100, 0),
(@PATH, 14, -10391.54, -1118.014, 22.62218, 0, 0, 0, 0, 100, 0),
(@PATH, 15, -10351.73, -1116.864, 21.51039, 0, 0, 0, 0, 100, 0),
(@PATH, 16, -10320.51, -1125.687, 21.68385, 0, 0, 0, 0, 100, 0);
-- 0x203CD4000000D20000003E0000461E16 .go xyz -10302.25 -1146.264 22.72622
