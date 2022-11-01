#include <memory.h>
#include <ec_mgr.h>
#include "gr_qbert_coily.h"
#include <OS/OSError.h>
#include <so/so_external_value_accesser.h>
#include <mt/mt_spline.h>
#include <hk/hk_math.h>
#include <mt/mt_trig.h>
#include <cm/cm_quake.h>
#include <snd/snd_system.h>
#include <mt/mt_prng.h>
#include <mt/mt_common.h>
#include <ft/ft_manager.h>

grQbertCoily* grQbertCoily::create(int mdlIndex, char* tgtNodeName, char* taskName, stMelee* stage, grQbertAlien* enemyTarget){
    grQbertCoily* coily = new(Heaps::StageInstance) grQbertCoily(taskName);
    coily->setMdlIndex(mdlIndex);
    coily->heapType = Heaps::StageInstance;
    coily->makeCalcuCallback(1, Heaps::StageInstance);
    coily->setCalcuCallbackRoot(7);
    coily->stage = stage;
    coily->enemyTarget = enemyTarget;

    coily->setupAttack();
    coily->setupHitPoint();

    return coily;
}

void grQbertCoily::setupAttack() {

    float size = 1.0;
    Vec3f offsetPos = {0.0, 0.0, 0.0};
    this->setAttack(size, &offsetPos);
    this->attackInfo->preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->reactionEffect = 0x40;
    overwriteAttackData->reactionFix = 0;
    overwriteAttackData->reactionAdd = 0;
    overwriteAttackData->power = 20;
    overwriteAttackData->vector = 90;
    overwriteAttackData->size = size;
    overwriteAttackData->offsetPos = offsetPos;
    overwriteAttackData->hitstopMultiplier = 1.0;

    overwriteAttackData->bits.nodeIndex = 0x1;

    overwriteAttackData->bits.isCollisionCategory9 = true;
    overwriteAttackData->bits.isCollisionCategory8 = true;
    overwriteAttackData->bits.isCollisionCategory7 = true;
    overwriteAttackData->bits.isCollisionCategory6 = true;
    overwriteAttackData->bits.isCollisionCategory5 = true;
    overwriteAttackData->bits.isCollisionCategory4 = true;
    overwriteAttackData->bits.isCollisionCategory3 = true;
    overwriteAttackData->bits.isCollisionCategory2 = true;
    overwriteAttackData->bits.isCollisionCategory1 = true;
    overwriteAttackData->bits.isCollisionCategory0 = true;

    overwriteAttackData->bits.isCollisionSituationUnk = true;
    overwriteAttackData->bits.isCollisionSituationAir = true;
    overwriteAttackData->bits.isCollisionSituationGround = true;

    overwriteAttackData->bits.field_0x30_3 = false;
    overwriteAttackData->bits.isCollisionPartRegion3 = true;
    overwriteAttackData->bits.isCollisionPartRegion2 = true;
    overwriteAttackData->bits.isCollisionPartRegion1 = true;
    overwriteAttackData->bits.isCollisionPartRegion0 = true;
    overwriteAttackData->bits.elementType = Element_Type_Normal;

    overwriteAttackData->bits.hitSoundLevel = Hit_Sound_Level_Small;
    overwriteAttackData->bits.hitSoundType = Hit_Sound_Type_Paper;
    overwriteAttackData->bits.isClankable = true;
    overwriteAttackData->bits.field_0x34_3 = false;
    overwriteAttackData->bits.field_0x34_4 = false;
    overwriteAttackData->bits.isBlockable = true;
    overwriteAttackData->bits.isReflectable = true;
    overwriteAttackData->bits.isAbsorbable = false;
    overwriteAttackData->bits.field_0x34_8 = 0;

    overwriteAttackData->bits.detectionRate = 0x3c;
    overwriteAttackData->bits.field_0x38_1 = false;
    overwriteAttackData->bits.ignoreInvincibility = false;
    overwriteAttackData->bits.ignoreIntangibility = false;
    overwriteAttackData->bits.facingRestriction = Facing_Restriction_Normal;
    overwriteAttackData->bits.field_0x38_5 = false;
    overwriteAttackData->bits.enableFriendlyFire = false;
    overwriteAttackData->bits.disableHitstop = false;
    overwriteAttackData->bits.disableHitGfx = false;
    overwriteAttackData->bits.disableFlinch = false;
    overwriteAttackData->bits.addedShieldDamage = 0;

    overwriteAttackData->bits.isShapeCapsule = false;
}

void grQbertCoily::setupHitPoint() {
    Vec3f startOffsetPos = {0,0,0};
    Vec3f endOffsetPos = {0,0,0};
    this->setHitPoint(7.0, &startOffsetPos, &endOffsetPos, 1, 1);
}

void grQbertCoily::setStartPos() {
    this->targetIndex = STARTING_CUBE_INDEX + 1 + randi(2);
    grQbertCube* cube = (grQbertCube*)this->stage->getGround(this->targetIndex);
    cube->getNodePosition(&this->targetPos, 0, "Jumps");
    this->prevPos = this->targetPos + (Vec3f){0, this->stage->deadRange.top, 0};
    this->midpointPos = this->prevPos;
    this->setPos(&this->prevPos);
}

void grQbertCoily::setStart() {
    this->yakumono->setTeam(15);
    this->setMotion(4);
    grQbertEnemy::setStart();
}

void grQbertCoily::updateMove(float frameDelta) {
    float animFrames = this->modelAnims[0]->getFrame();
    float animFrameCount = this->modelAnims[0]->getFrameCount();
    float jumpCompletion = animFrames / animFrameCount;

    if (this->timer > 0) {
        this->timer -= frameDelta;
        if (this->timer <= 0) {
            this->setAnim();
        }
    }
    else if (jumpCompletion <= 1.0) { // Mid jump
        Vec3f pos;
        Vec3f points[4] = {
                this->prevPos,
                this->midpointPos,
                this->midpointPos,
                this->targetPos
        };
        mtBezierCurve(jumpCompletion, points, &pos);
        pos = pos + this->shakeOffset;
        this->setPos(&pos);
    }
    else if (animFrames - animFrameCount > JUMP_WAIT_FRAMES) { // Pick new target
        this->setTargetPos();
    }
}

void grQbertCoily::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {

}

void grQbertCoily::setTargetPos() {
    this->prevPos = this->targetPos;

    // get next cube target based on nodes
    grQbertCube* cube = (grQbertCube*)this->stage->getGround(this->targetIndex);
    u32 numJumps = cube->getNumNextJumpCubes();
    u32 cubeIndices[MAX_JUMPS];
    cube->getNextJumpCubes(cubeIndices);

    if (!isHatched) {
        u32 validCubeIndices[MAX_JUMPS];
        u32 numValidJumps = 0;
        for (u32 i = 0; i < numJumps; i++) {
            Vec3f pos;
            cube = (grQbertCube*)stage->getGround(cubeIndices[i] - STARTING_CUBE_INDEX);
            cube->getNodePosition(&pos, 0, "Jumps");
            if (pos.y < this->prevPos.y) {
                validCubeIndices[numValidJumps] = cubeIndices[i];
                numValidJumps++;
            }
        }
        if (numValidJumps > 0) {
            this->targetIndex = validCubeIndices[randi(numValidJumps)] - STARTING_CUBE_INDEX;
        }
        else {
            this->timer = HATCH_FRAMES;
            isHatched = true;
        }
    }
    if (isHatched) {
        Vec3f targetPos = {0, 0, 0};
        if (this->enemyTarget->getLives() > 0) {
            // Pursue QBert
            targetPos = this->enemyTarget->targetPos;
        }
        else {
            // Pursue closest player

            // Find closest player pos to snake's current pos
            float minDist = 100000000000000;
            for (u8 i = 0; i < g_ftManager->getEntryCount(); i++) {
                Fighter* fighter = g_ftManager->getFighter(g_ftManager->getEntryIdFromIndex(i), 0);
                Vec3f fighterPos = soExternalValueAccesser::getPos((StageObject*)fighter);
                float dist = distance(&fighterPos, &this->prevPos);
                if (dist < minDist) {
                    minDist = dist;
                    targetPos = fighterPos;
                }
            }
        }

        // Find block that will lead to getting closest to the target
        float minDistToTarget = 100000000000000;
        for (u32 i = 0; i < numJumps; i++) {
            Vec3f pos;
            u32 cubeIndex = cubeIndices[i] - STARTING_CUBE_INDEX;
            cube = (grQbertCube*)stage->getGround(cubeIndex);
            cube->getNodePosition(&pos, 0, "Jumps");
            float dist = distance(&pos, &targetPos);
            if (dist < minDistToTarget) {
                minDistToTarget = dist;
                this->targetIndex = cubeIndex;
            }
        }
    }

    cube = (grQbertCube*)stage->getGround(this->targetIndex);
    cube->getNodePosition(&this->targetPos, 0, "Jumps");
    this->midpointPos = (Vec3f){(this->prevPos.x + this->targetPos.x)/2, hkMath::max2f(this->prevPos.y, this->targetPos.y) + 5, (this->prevPos.z + this->targetPos.z)/2};

    this->setAnim();
}

void grQbertCoily::setAnim() {
    if (this->isHatched) {
        grQbertEnemy::setAnim();
    }
    else {
        this->setMotion(5);
    }
}