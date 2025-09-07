#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_qbert_ball.h"
#include <OS/OSError.h>
#include <so/so_external_value_accesser.h>
#include <mt/mt_spline.h>
#include <hkmath/hkMath.h>
#include <mt/mt_trig.h>
#include <cm/cm_quake.h>
#include <snd/snd_system.h>
#include <mt/mt_prng.h>
#include <mt/mt_common.h>
#include <ft/ft_manager.h>
#include <string.h>

void grQbertBall::setStartPos() {
    this->targetIndex = STARTING_CUBE_INDEX + 1 + randi(2);
    grQbertCube* cube = (grQbertCube*)this->stage->getGround(this->targetIndex);
    cube->getNodePosition(&this->targetPos, 0, "Jumps");
    this->prevPos = this->targetPos + Vec3f(0, this->stage->m_deadRange.m_up + 10, 0);
    this->midpointPos = this->prevPos;
    this->setPos(&this->prevPos);
}

void grQbertBall::setTargetPos() {
    Vec3f deltaPos = this->targetPos - this->prevPos;
    this->prevPos = this->targetPos;

    // get next cube target based on nodes
    grQbertCube* cube = (grQbertCube*)this->stage->getGround(this->targetIndex);
    u32 numJumps = cube->getNumNextJumpCubes();
    u32 cubeIndices[MAX_JUMPS];
    cube->getNextJumpCubes(cubeIndices);

    u32 validCubeIndices[MAX_JUMPS];
    u32 numValidJumps = 0;
    // check for potential jumps lower than current position
    for (u32 i = 0; i < numJumps; i++) {
        Vec3f pos;
        cube = (grQbertCube*)stage->getGround(cubeIndices[i]);
        cube->getNodePosition(&pos, 0, "Jumps");
        if (pos.m_y < this->prevPos.m_y) {
            validCubeIndices[numValidJumps] = cubeIndices[i];
            numValidJumps++;
        }
    }
    if (numValidJumps > 0) {
        this->targetIndex = validCubeIndices[randi(numValidJumps)];
        cube = (grQbertCube*)stage->getGround(this->targetIndex);
        cube->getNodePosition(&this->targetPos, 0, "Jumps");
    }
    else {
        // fall off
        this->targetPos = this->prevPos + deltaPos;
        this->targetPos.m_y = this->stage->m_deadRange.m_down;
    }

    this->midpointPos = Vec3f((this->prevPos.m_x + this->targetPos.m_x)/2, hkMath::max2(this->prevPos.m_y, this->targetPos.m_y) + 5, (this->prevPos.m_z + this->targetPos.m_z)/2);
    this->setAnim();
}

void grQbertBall::setAnim() {
    this->setMotion(1);
}