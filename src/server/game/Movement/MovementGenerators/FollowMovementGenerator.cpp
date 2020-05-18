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

#include "FollowMovementGenerator.h"
#include "FollowerHandler.h"
#include "CreatureAI.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "PathGenerator.h"
#include "Pet.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "Util.h"
#include <optional>

/*
TODO

- SUMMON_SLOT_QUEST (6) --> always move behind the player
- Lunk --> can catch up, fandral cannot (both slot 6, allied, flags?)

*/

FollowMovementGenerator::FollowMovementGenerator(Unit* target, float range, ChaseAngle angle, bool allignToTargetSpeed, bool faceTarget) :
    _target(target), _range(range), _useTargetSpeed(allignToTargetSpeed), _faceTarget(faceTarget), _returnState(RETURN_STATE_RETURNED), _angle(angle) { }

FollowMovementGenerator::~FollowMovementGenerator() { }

inline UnitMoveType SelectSpeedType(uint32 moveFlags)
{
    if (moveFlags & MOVEMENTFLAG_FLYING)
    {
        if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.flight >= speed_obj.flight_back*/)
            return MOVE_FLIGHT_BACK;
        else
            return MOVE_FLIGHT;
    }
    else if (moveFlags & MOVEMENTFLAG_SWIMMING)
    {
        if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.swim >= speed_obj.swim_back*/)
            return MOVE_SWIM_BACK;
        else
            return MOVE_SWIM;
    }
    else if (moveFlags & MOVEMENTFLAG_WALKING)
    {
        //if (speed_obj.run > speed_obj.walk)
        return MOVE_WALK;
    }
    else if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.run >= speed_obj.run_back*/)
        return MOVE_RUN_BACK;

    // Flying creatures use MOVEMENTFLAG_CAN_FLY or MOVEMENTFLAG_DISABLE_GRAVITY
    // Run speed is their default flight speed.
    return MOVE_RUN;
}

static void DoMovementInform(Unit* owner, Unit* target)
{
    if (owner->GetTypeId() != TYPEID_UNIT)
        return;

    if (UnitAI* ai = owner->GetAI())
        static_cast<CreatureAI*>(ai)->MovementInform(FOLLOW_MOTION_TYPE, target->GetGUID().GetCounter());
}

void FollowMovementGenerator::Initialize(Unit* owner)
{
    owner->AddUnitState(UNIT_STATE_FOLLOW);
    _followMovementTimer.Reset(0);
}

bool FollowMovementGenerator::Update(Unit* owner, uint32 diff)
{
    // Owner might be dead or gone
    if (!owner->IsAlive())
        return false;

    // Our target might have gone away
    Unit* const target = _target;
    if (!target || !target->IsInWorld())
        return false;

    // Follower cannot move at the moment
    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting())
    {
        owner->StopMoving();
        _returnState = RETURN_STATE_NONE;
        _followMovementTimer.Reset(0);
        return true;
    }

    if (!owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE) && !_faceTarget)
    {
        _allignMovementTimer.Update(diff);
        _facingMovementTimer.Update(diff);

        if (_allignMovementTimer.Passed() && _returnState == RETURN_STATE_NONE)
        {
            // Return to our follow target
            Position dest = target->GetPosition();
            target->MovePositionToFirstCollision(dest, _range, _angle.RelativeAngle);

            // Launch a alligning spline when we are too far away from our follow target
            if (owner->GetExactDist2d(dest) > 1.f)
            {
                Movement::MoveSplineInit init(owner);
                init.MoveTo(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ());
                init.SetVelocity(target->GetSpeed(SelectSpeedType(target->GetUnitMovementFlags())));
                init.Launch();
                _returnState = RETURN_STATE_RETURNING;
                _facingMovementTimer.Reset(FOLLOW_MOVEMENT_INTERVAL);
            }
            else
                _returnState = RETURN_STATE_RETURNED;
        }

        if (_facingMovementTimer.Passed() && _returnState == RETURN_STATE_RETURNING)
        {
            if (owner->movespline->Finalized())
            {
                owner->SetFacingTo(target->GetOrientation());
                _returnState = RETURN_STATE_RETURNED;
            }
            else
                _facingMovementTimer.Reset(FOLLOW_MOVEMENT_INTERVAL);
        }

        _followMovementTimer.Reset(0);
    }

    _followMovementTimer.Update(diff);
    if (_followMovementTimer.Passed())
    {
        // Target is currently moving, launch our next spline
        if (!target->movespline->Finalized() || target->isMoving())
            LaunchMovement(owner, target, false);
        else if (owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE) && target->movespline->Finalized() && !target->isMoving())
        {
            // Owner is no longer moving. Prepare out allignment movement
            if (!_faceTarget)
            {
                owner->StopMoving();
                owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
                _allignMovementTimer.Reset(ALLING_MOVEMENT_INTERVAL);
                DoMovementInform(owner, target);
            }
            else
                LaunchMovement(owner, target, true);
        }

        _followMovementTimer.Reset(FOLLOW_MOVEMENT_INTERVAL);
    }

    return true;
}

void FollowMovementGenerator::Finalize(Unit* owner)
{
    owner->ClearUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
    if (_target)
        _target->GetFollowerHandler()->RemoveFollower(owner);
}

void FollowMovementGenerator::SetFollowParameters(float distance, float relativeAngle)
{
    _range = distance;
    _angle = relativeAngle;

    // Prepare a new movement
    _allignMovementTimer.Reset(ALLING_MOVEMENT_INTERVAL);
    _returnState = RETURN_STATE_NONE;
};

void FollowMovementGenerator::LaunchMovement(Unit* owner, Unit* target, bool stopped)
{
    uint32 movementFlags = target->GetUnitMovementFlags();
    if (!stopped && !target->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))
        return;

    UnitMoveType moveType = SelectSpeedType(movementFlags);
    Position dest = target->GetPosition();

    float offset = 0.f;

    // Strafe handling for player sidewards movement
    if (target->IsPlayer())
    {
        if (target->HasUnitMovementFlag(MOVEMENTFLAG_STRAFE_LEFT))
            offset += float(M_PI / 2);

        if (target->HasUnitMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT))
            offset -= float(M_PI / 2);

        if (target->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD))
            offset += float(M_PI);

        // Player moves forward while strafing, cut strafe offset in half
        if (offset != 0.f && target->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD))
            offset *= 0.5f;
    }

    float angle = !target->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD) ? _angle.RelativeAngle : -_angle.RelativeAngle;

    // Let's start with a cheap base destination calculation
    dest.m_positionX += std::cos(Position::NormalizeOrientation(target->GetOrientation() + offset + angle)) * _range;
    dest.m_positionY += std::sin(Position::NormalizeOrientation(target->GetOrientation() + offset + angle)) * _range;

    // Determine our velocity
    float velocity = 0.f;
    if (_useTargetSpeed)
    {
        if (target->IsPlayer())
            velocity = target->GetSpeed(SelectSpeedType(target->m_movementInfo.GetMovementFlags()));
        else
        {
            if (!target->movespline->Finalized())
                velocity = target->movespline->Velocity();
            else
                velocity = target->GetSpeed(MOVE_RUN);
        }
    }

    // Catchup / throttle speed modifier calculation
    if (_useTargetSpeed && velocity > 0.f)
    {
        float distance = owner->GetExactDist2d(dest);
        if (distance > 0.5f)
        {
            float distMod = 1.f;
            if (!dest.HasInArc(float(M_PI), owner)) // Follower is in falling back, catch up
                distMod += std::min<float>(distance * 0.1f, 0.5f);
            else if (distance > (velocity / 2))  // Follower is beyond destination, fall back
                distMod -= std::min<float>((distance - (velocity / 2)), 0.5f);

            velocity *= distMod;
        }
    }

    // Predicting our follow destination
    if (!stopped)
    {
        dest.m_positionX += std::cos(Position::NormalizeOrientation(target->GetOrientation() + offset)) * (velocity * 2);
        dest.m_positionY += std::sin(Position::NormalizeOrientation(target->GetOrientation() + offset)) * (velocity * 2);
    }

    // Now we calculate out actual destination data
    if (!owner->HasUnitState(UNIT_STATE_IGNORE_PATHFINDING))
    {
        float relativeAngle = target->GetRelativeAngle(dest);
        float distance = target->GetExactDist2d(dest);
        dest = target->GetPosition();
        target->MovePositionToFirstCollision(dest, distance, relativeAngle);
    }

    Movement::MoveSplineInit init(owner);
    init.MoveTo(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ());

    if (velocity > 0.f)
        init.SetVelocity(velocity);

    if (_faceTarget)
        init.SetFacing(target);

    init.Launch();

    if (!stopped)
    {
        owner->AddUnitState(UNIT_STATE_FOLLOW_MOVE);
        _returnState = RETURN_STATE_NONE;
    }
}
