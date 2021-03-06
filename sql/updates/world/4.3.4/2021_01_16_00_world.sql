SET @CGUID := 291429;
SET @PATH := @CGUID * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -358.6962, 1069.132, 21.85921, 0, 129),
(@PATH, 1, -350.6337, 1072.78, 21.53023, 0, -716),
(@PATH, 2, -343.7083, 1073.292, 21.35921, 0, -1057),
(@PATH, 3, -338.7743, 1068.306, 21.49336, 0, -1454),
(@PATH, 4, -334.0365, 1060.42, 21.86575, 0, -1508),
(@PATH, 5, -332.5243, 1054.642, 22.02245, 0, -1439),
(@PATH, 6, -336.3333, 1045.595, 21.85378, 0, -1741),
(@PATH, 7, -344.3264, 1042.731, 21.56594, 0, -1463),
(@PATH, 8, -354.3264, 1043.92, 21.88772, 0, -651),
(@PATH, 9, -359.6632, 1045.901, 21.88772, 0, -1706),
(@PATH, 10, -363.1406, 1049.899, 21.90212, 0, -1374),
(@PATH, 11, -366.533, 1056.254, 22.01272, 0, -630),
(@PATH, 12, -367.4618, 1060.483, 21.80136, 0, -1140),
(@PATH, 13, -363.8108, 1065.535, 21.88772, 0, -1189),
(@PATH, 14, -358.8576, 1068.865, 21.85921, 0, 39),
(@PATH, 15, -363.8108, 1065.535, 21.88772, 0, -1169),
(@PATH, 16, -367.4618, 1060.483, 21.80136, 0, -1232),
(@PATH, 17, -366.533, 1056.254, 22.01272, 0, -1755),
(@PATH, 18, -363.1406, 1049.899, 21.90212, 0, -987),
(@PATH, 19, -359.6632, 1045.901, 21.88772, 0, -663),
(@PATH, 20, -354.3264, 1043.92, 21.88772, 0, -1751),
(@PATH, 21, -344.3264, 1042.731, 21.56594, 0, -885),
(@PATH, 22, -336.3333, 1045.595, 21.85378, 0, -616),
(@PATH, 23, -332.5243, 1054.642, 22.02245, 0, -906),
(@PATH, 24, -334.0365, 1060.42, 21.86575, 0, -845),
(@PATH, 25, -338.7743, 1068.306, 21.49336, 0, -906),
(@PATH, 26, -343.7083, 1073.292, 21.35921, 0, -1253),
(@PATH, 27, -350.6337, 1072.78, 21.53023, 0, -1612);

UPDATE `creature` SET `position_x`= -358.6962, `position_y`= 1069.132, `position_z`= 21.85921, `orientation`= 0, `spawndist`= 0, `MovementType`= 2 WHERE `guid`= @CGUID;
DELETE FROM `creature_addon` WHERE `guid`= @CGUID;
INSERT INTO `creature_addon` (`guid`, `waypointPathId`, `bytes2`) VALUES
(@CGUID, @PATH, 1);

SET @CGUID := 291158;
SET @PATH := @CGUID * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -632.151, 1648.88, 47.38609, 0, 245),
(@PATH, 1, -642.931, 1654.93, 47.37844, 0, -1293),
(@PATH, 2, -652.233, 1662.75, 47.90628, 0, -1299),
(@PATH, 3, -652.335, 1670.94, 48.63116, 0, -923),
(@PATH, 4, -657.148, 1678.91, 48.38555, 0, -1010),
(@PATH, 5, -661.542, 1687.02, 48.48833, 0, -1050),
(@PATH, 6, -667.462, 1685.63, 47.58488, 0, -1201),
(@PATH, 7, -675.51, 1719.13, 48.61787, 0, 149),
(@PATH, 8, -667.462, 1685.63, 47.58488, 0, -871),
(@PATH, 9, -661.627, 1687.001, 48.43096, 0, -841),
(@PATH, 10, -657.148, 1678.91, 48.38555, 0, -937),
(@PATH, 11, -652.335, 1670.94, 48.63116, 0, -1059),
(@PATH, 12, -652.233, 1662.75, 47.90628, 0, -690),
(@PATH, 13, -642.931, 1654.93, 47.37844, 0, -625);

UPDATE `creature` SET `position_x`= -632.151, `position_y`= 1648.88, `position_z`= 47.38609, `orientation`= 0, `spawndist`= 0, `MovementType`= 2 WHERE `guid`= @CGUID;
DELETE FROM `creature_addon` WHERE `guid`= @CGUID;
INSERT INTO `creature_addon` (`guid`, `waypointPathId`, `bytes2`) VALUES
(@CGUID, @PATH, 1);
