#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_qbert_coily.h"
#include <OS/OSError.h>
#include <ft/ft_external_value_accesser.h>
#include <mt/mt_spline.h>
#include <hk/hk_math.h>
#include <mt/mt_trig.h>
#include <cm/cm_quake.h>
#include <snd/snd_system.h>
#include <mt/mt_prng.h>
#include <mt/mt_common.h>
#include <ft/ft_manager.h>
#include <string.h>

grQbertCoily* grQbertCoily::create(int mdlIndex, char* tgtNodeName, char* taskName, stMelee* stage, grQbertAlien* enemyTarget){
    grQbertCoily* coily = new(Heaps::StageInstance) grQbertCoily(taskName);
    coily->setMdlIndex(mdlIndex);
    coily->m_heapType = Heaps::StageInstance;
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
    this->m_attackInfo->m_preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = 0x40;
    overwriteAttackData->m_reactionFix = 0;
    overwriteAttackData->m_reactionAdd = 0;
    overwriteAttackData->m_power = 20;
    overwriteAttackData->m_vector = 90;
    overwriteAttackData->m_size = size;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitstopMultiplier = 1.0;

    overwriteAttackData->m_bits.nodeIndex = 0x1;

    overwriteAttackData->m_bits.isCollisionCategory9 = true;
    overwriteAttackData->m_bits.isCollisionCategory8 = true;
    overwriteAttackData->m_bits.isCollisionCategory7 = true;
    overwriteAttackData->m_bits.isCollisionCategory6 = true;
    overwriteAttackData->m_bits.isCollisionCategory5 = true;
    overwriteAttackData->m_bits.isCollisionCategory4 = true;
    overwriteAttackData->m_bits.isCollisionCategory3 = true;
    overwriteAttackData->m_bits.isCollisionCategory2 = true;
    overwriteAttackData->m_bits.isCollisionCategory1 = true;
    overwriteAttackData->m_bits.isCollisionCategory0 = true;

    overwriteAttackData->m_bits.isCollisionSituationODD = true;
    overwriteAttackData->m_bits.isCollisionSituationAir = true;
    overwriteAttackData->m_bits.isCollisionSituationGround = true;

    overwriteAttackData->m_bits.field_0x30_3 = false;
    overwriteAttackData->m_bits.isCollisionPartRegion3 = true;
    overwriteAttackData->m_bits.isCollisionPartRegion2 = true;
    overwriteAttackData->m_bits.isCollisionPartRegion1 = true;
    overwriteAttackData->m_bits.isCollisionPartRegion0 = true;
    overwriteAttackData->m_bits.elementType = Element_Type_Normal;

    overwriteAttackData->m_bits.hitSoundLevel = Hit_Sound_Level_Small;
    overwriteAttackData->m_bits.hitSoundType = Hit_Sound_Type_NoneUnique;
    overwriteAttackData->m_bits.isClankable = true;
    overwriteAttackData->m_bits.field_0x34_3 = false;
    overwriteAttackData->m_bits.field_0x34_4 = false;
    overwriteAttackData->m_bits.isBlockable = true;
    overwriteAttackData->m_bits.isReflectable = true;
    overwriteAttackData->m_bits.isAbsorbable = false;
    overwriteAttackData->m_bits.field_0x38_10 = 0;

    overwriteAttackData->m_bits.detectionRate = 120;
    overwriteAttackData->m_bits.field_0x38_1 = false;
    overwriteAttackData->m_bits.ignoreInvincibility = false;
    overwriteAttackData->m_bits.ignoreIntangibility = false;
    overwriteAttackData->m_bits.facingRestriction = Facing_Restriction_Normal;
    overwriteAttackData->m_bits.field_0x38_5 = false;
    overwriteAttackData->m_bits.enableFriendlyFire = false;
    overwriteAttackData->m_bits.disableHitstop = false;
    overwriteAttackData->m_bits.disableHitGfx = false;
    overwriteAttackData->m_bits.disableFlinch = false;
    overwriteAttackData->m_bits.addedShieldDamage = 0;

    overwriteAttackData->m_bits.isShapeCapsule = false;
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
    this->prevPos = this->targetPos + (Vec3f){0, this->stage->m_deadRange.m_top + 10, 0};
    this->midpointPos = this->prevPos;
    this->setPos(&this->prevPos);
}

void grQbertCoily::setStart() {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    this->timer = randf()*(qbertStageData->coilyMaxRespawnFrames - qbertStageData->coilyMinRespawnFrames) + qbertStageData->coilyMinRespawnFrames;
    this->m_yakumono->setTeam(15);
    this->isHatched = false;
    this->setMotion(4);
    this->damage = 0;
    grQbertEnemy::setStart();
}

void grQbertCoily::updateMove(float frameDelta) {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    float animFrames = this->m_modelAnims[0]->getFrame();
    float animFrameCount = this->m_modelAnims[0]->getFrameCount();
    float jumpCompletion = animFrames / animFrameCount;

    if (this->isDead) { // Launched
        this->timer += frameDelta;
        if (this->timer == qbertStageData->knockoutFrames) {
            this->soundGenerator.playSE(snd_se_stage_Madein_08, 0x0, 0x0, 0xffffffff);
            cmReqQuake(1, &(Vec3f){0,0,0});
            if (this->attackerEntryId >= 0) {
               int team = g_ftManager->getTeam(this->attackerEntryId, false, false);
                this->teamScoresWork[team] += COILY_POINTS;
                if (this->gameRule == Game_Rule_Coin) {
                    g_ftManager->pickupCoin(this->attackerEntryId, COILY_POINTS);
               }
            }
        }
        Vec3f pos = this->getPos();
        stRange* range = &this->stage->m_deadRange;
        if (pos.m_x <= range->m_left || pos.m_x >= range->m_right || pos.m_y >= range->m_top || pos.m_y <= range->m_bottom) {
            if (this->isHatched && this->timer >= qbertStageData->knockoutFrames) {
                this->setStart();
            }
            else if (!this->isHatched && this->timer >= qbertStageData->dropFrames) {
                this->setStart();
            }
        }
        else {
            Vec3f rot = this->getRot();
            rot.m_z += this->velocity * frameDelta;
            this->setRot(&rot);
            Vec3f pos;
            Vec3f points[4] = {
                    this->prevPos,
                    this->midpointPos,
                    this->midpointPos,
                    this->targetPos
            };
            float completion = this->timer / qbertStageData->knockoutFrames;
            if (!this->isHatched) {
                    points[1] = this->prevPos;
                    points[2] = this->targetPos;
                    completion = this->timer / qbertStageData->dropFrames;
            }
            mtBezierCurve(completion, points, &pos);
            this->setPos(&pos);
        }
    }
    else if (this->timer > 0) { // Respawning
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
    else if (animFrames - animFrameCount <= 1.0) { // Landed
        Vec3f pos = this->targetPos + this->shakeOffset;
        this->setPos(&pos);
        if (!this->isHatched) {
            this->soundGenerator.playSE(snd_se_stage_Madein_03, 0x0, 0x0, 0xffffffff);
        }
    }
    else if (animFrames - animFrameCount > qbertStageData->jumpWaitFrames) { // Pick new target
        if (this->isHatched) {
            this->soundGenerator.playSE(snd_se_stage_Madein_09, 0x0, 0x0, 0xffffffff);
        }
        this->setTargetPos();
    }
    else {
        Vec3f pos = this->targetPos + this->shakeOffset;
        this->setPos(&pos);
    }
}

void grQbertCoily::onInflictEach(soCollisionLog* collisionLog, float power) {
    if (this->isHatched) {
        this->soundGenerator.playSE(snd_se_stage_Madein_02, 0x0, 0x0, 0xffffffff);
        // Check if Coily hit QBert
        if (strcmp(gfTask::getTask(collisionLog->m_taskId)->m_taskName, "ykNormal") == 0) {
            this->setStart();
        }
    }
    else {
        this->soundGenerator.playSE(snd_se_stage_Madein_17, 0x0, 0x0, 0xffffffff);
    }
}

void grQbertCoily::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    damage->m_totalDamage = 0;
    this->damage += damage->m_damage;
    if ((!this->isHatched && this->damage > qbertStageData->coilyEggHP) || (this->isHatched && this->damage > qbertStageData->coilySnakeHP)) {
        this->timer = 0;
        this->setSleepAttack(true);
        this->setSleepHit(true);
        this->isDead = true;
        this->angle = damage->m_vector;
        if (damage->m_side == -1) {
            this->angle = 180 - damage->m_vector;
        }
        this->velocity = damage->m_reaction / 60;

        this->prevPos = this->getPos();
        if (this->isHatched) {
            this->setMotionRatio(0.0);
            this->targetPos = (Vec3f){this->prevPos.m_x, this->stage->m_deadRange.m_bottom, -500};
            this->midpointPos = (Vec3f){this->prevPos.m_x, 110, this->prevPos.m_z};
            this->soundGenerator.playSE(snd_se_stage_Madein_good_03, 0x0, 0x0, 0xffffffff);
        }
        else {
            this->targetPos = (Vec3f){this->prevPos.m_x, this->stage->m_deadRange.m_bottom, 0};
        }
    }
    if (attackerInfo->m_indirectAttackerSoKind == SoKind_Fighter) {
        this->attackerEntryId = attackerInfo->m_indirectAttackerEntryId;
    }
    else {
        this->attackerEntryId = -1;
    }

}

void grQbertCoily::setTargetPos() {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    this->prevPos = this->targetPos;

    // get next cube target based on nodes
    grQbertCube* cube = (grQbertCube*)this->stage->getGround(this->targetIndex);
    u32 numJumps = cube->getNumNextJumpCubes();
    u32 cubeIndices[MAX_JUMPS];
    cube->getNextJumpCubes(cubeIndices);

    if (!this->isHatched) {
        u32 validCubeIndices[MAX_JUMPS];
        u32 numValidJumps = 0;
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
        }
        else {
            this->timer = qbertStageData->coilyHatchFrames;
            this->isHatched = true;
        }
    }
    if (this->isHatched) {
        Vec3f targetPos = {0, 0, 0};
        if (!this->enemyTarget->isDead) {
            // Pursue QBert
            targetPos = this->enemyTarget->targetPos;
        }
        else {
            // Pursue closest player

            // Find closest player pos to snake's current pos
            Fighter* fighter = g_ftManager->searchNearFighter(0.0, -1.0, &this->prevPos, -1, false);
            if (fighter != NULL) {
                targetPos = ftExternalValueAccesser::getHipPos(fighter);
            }
        }

        // Find block that will lead to getting closest to the target
        float minDistToTarget = 100000000000000;
        for (u32 i = 0; i < numJumps; i++) {
            Vec3f pos;
            u32 cubeIndex = cubeIndices[i];
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
    this->midpointPos = (Vec3f){(this->prevPos.m_x + this->targetPos.m_x)/2, hkMath::max2f(this->prevPos.m_y, this->targetPos.m_y) + 5, (this->prevPos.m_z + this->targetPos.m_z)/2};

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