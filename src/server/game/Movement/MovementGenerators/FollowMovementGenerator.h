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

#ifndef TRINITY_FOLLOWMOVEMENTGENERATOR_H
#define TRINITY_FOLLOWMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "Optional.h"
#include "Timer.h"

class PathGenerator;
class Unit;

enum ReturnState
{
    RETURN_STATE_NONE = 0,
    RETURN_STATE_RETURNING,
    RETURN_STATE_RETURNED
};

class FollowMovementGenerator : public MovementGenerator
{
public:
    MovementGeneratorType GetMovementGeneratorType() const override { return FOLLOW_MOTION_TYPE; }

    FollowMovementGenerator(Unit* target, float range, ChaseAngle angle, bool alligntToTargetSpeed = false, bool faceTarget = false);
    ~FollowMovementGenerator();

    void Initialize(Unit* owner) override;
    void Reset(Unit* owner) override { Initialize(owner); }
    bool Update(Unit* owner, uint32 diff) override;
    void Finalize(Unit* owner) override;

    void SetFollowParameters(float distance, float relativeAngle);

private:
    void LaunchMovement(Unit* owner, Unit* target, bool stopped);

    static constexpr uint32 FOLLOW_MOVEMENT_INTERVAL = 400; // sniffed (1 batch update cycle)
    static constexpr uint32 ALLING_MOVEMENT_INTERVAL = 2000; // sniffed (5 batch update cycles)
    // static inline const when?

    Unit* _target;
    float _range;
    bool const _useTargetSpeed;
    bool const _faceTarget;
    ReturnState _returnState;
    ChaseAngle _angle;
    TimeTrackerSmall _followMovementTimer;
    TimeTrackerSmall _allignMovementTimer;
    TimeTrackerSmall _facingMovementTimer;
};

#endif
