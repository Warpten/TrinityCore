/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FollowerHandler_h__
#define FollowerHandler_h__

#include "Define.h"
#include "OptionalFwd.h"
#include <vector>

class Unit;
struct Position;

constexpr uint8 MAX_FOLLOWERS_PER_ROW       = 6;
constexpr uint8 DEFAULT_ROW_FOLLOWERS       = 2;
constexpr float STRAIGHT_FOLLOW_DISTANCE    = 3.f;
constexpr float SIDE_FOLLOW_DISTANCE        = 3.3541f;


class TC_GAME_API FollowerHandler
{
public:
    FollowerHandler(Unit* target);

    void ReorganizeFollowers();
    void AddFollower(Unit* follower, bool catchUp = true, Optional<float> distance = nullptr, Optional<float> angle = nullptr);
    void RemoveFollower(Unit* follower);
    void RemoveAllFollowers();
    void GetFollowOffsets(uint8 followerIndex, float& distance, float& relativeAngle);

private:
    Unit* _followTarget;
    std::vector<Unit*> _followers;
};

#endif // FollowerHandler_h__
