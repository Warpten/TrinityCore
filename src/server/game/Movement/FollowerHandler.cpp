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

#include "FollowerHandler.h"
#include "FollowMovementGenerator.h"
#include "Unit.h"
#include "TemporarySummon.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"

FollowerHandler::FollowerHandler(Unit* target) : _followTarget(target) { }

void FollowerHandler::ReorganizeFollowers()
{
    uint8 followerCount = 0;
    for (std::vector<Unit*>::const_iterator itr = _followers.begin(); itr != _followers.end(); itr++)
    {
        Unit* follower = *itr;
        if (!follower) // this should never happen at this point but just in case
            continue;

        float angle = 0.f;
        float dist = 0.f;
        GetFollowOffsets(followerCount, dist, angle);

        if (MovementGenerator* gen = follower->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_IDLE))
            if (gen->GetMovementGeneratorType() == FOLLOW_MOTION_TYPE)
                static_cast<FollowMovementGenerator*>(gen)->SetFollowParameters(dist, angle);

        ++followerCount;
    }
}

void FollowerHandler::AddFollower(Unit* follower, bool catchUp /*= true*/, Optional<float> distance /*= nullptr*/, Optional<float> angle /*= nullptr*/)
{
    float followAngle = 0.f;
    float followDistance = 0.f;
    GetFollowOffsets(_followers.size(), followDistance, followAngle);

    _followers.push_back(follower);
    follower->GetMotionMaster()->MoveFollow(_followTarget, distance ? distance.get() : followDistance, angle ? angle.get() : followAngle, true);
}

void FollowerHandler::RemoveFollower(Unit* follower)
{
    bool followerRemoved = false;
    for (std::vector<Unit*>::const_iterator itr = _followers.begin(); itr != _followers.end();)
    {
        if ((*itr) == follower)
        {
            itr = _followers.erase(itr);
            followerRemoved = true;
        }
        else
            itr++;
    }

    // Update our follower movement slots
    if (followerRemoved)
        ReorganizeFollowers();
}

void FollowerHandler::RemoveAllFollowers()
{
    for (Unit* follower : _followers)
    {
        if (!follower)
            continue;

        if (follower->GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_IDLE) == FOLLOW_MOTION_TYPE)
            follower->GetMotionMaster()->Clear(MOTION_SLOT_IDLE);
    }

    _followers.clear();
}

void FollowerHandler::GetFollowOffsets(uint8 followerIndex, float& distance, float& relativeAngle)
{
    uint8 rowOffset = 0;

    if (followerIndex >= DEFAULT_ROW_FOLLOWERS)
        rowOffset = std::floor<uint8>((followerIndex - DEFAULT_ROW_FOLLOWERS) / MAX_FOLLOWERS_PER_ROW);
    else
    {
        // Default left and right follower slots
        distance = STRAIGHT_FOLLOW_DISTANCE;
        relativeAngle = followerIndex == 0 ? float(M_PI_2) : float(-M_PI_2);
        return;
    }

    // Further follower slots. Calculating a place in further rows
    uint8 rowIndex = (followerIndex - DEFAULT_ROW_FOLLOWERS) % MAX_FOLLOWERS_PER_ROW;

    // Blizzard assigns new followers in a left-right repeat pattern rather than just going from left to right
    bool left = rowIndex % 2;
    Position pos;
    pos.RelocateOffset({ 0.f, left ? -STRAIGHT_FOLLOW_DISTANCE : STRAIGHT_FOLLOW_DISTANCE });

    if (rowIndex <= 3)
    {
        pos.RelocateOffset({ -(STRAIGHT_FOLLOW_DISTANCE * rowOffset) });

        // Inner and outer positions
        float angle = float(M_PI);
        if (rowIndex <= 1)
            angle += left ? float(-(M_PI_4 / 2)) : float(M_PI_4 / 2);
        else
            angle += left ? float(M_PI_4 / 2) : float(-(M_PI_4 / 2));

        pos.m_positionX += std::cos(angle) * SIDE_FOLLOW_DISTANCE;
        pos.m_positionY += std::sin(angle) * SIDE_FOLLOW_DISTANCE;
    }
    else
    {
        // Middle positions
        pos.RelocateOffset({ -(STRAIGHT_FOLLOW_DISTANCE + STRAIGHT_FOLLOW_DISTANCE * rowOffset) });
    }

    distance = Position().GetExactDist2d(pos);
    relativeAngle = Position().GetRelativeAngle(pos);
}
