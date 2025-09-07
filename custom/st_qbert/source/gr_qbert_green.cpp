#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_qbert_green.h"
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

grQbertGreen* grQbertGreen::create(int mdlIndex, const char* tgtNodeName, const char* taskName, stMelee* stage){
    grQbertGreen* ball = new(Heaps::StageInstance) grQbertGreen(taskName);
    ball->setMdlIndex(mdlIndex);
    ball->m_heapType = Heaps::StageInstance;
    ball->makeCalcuCallback(1, Heaps::StageInstance);
    ball->setCalcuCallbackRoot(7);
    ball->stage = stage;

    ball->setupAttack();

    return ball;
}

void grQbertGreen::setupAttack() {

    float size = 4.0;
    Vec3f offsetPos = Vec3f(0.0, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = 0;
    overwriteAttackData->m_reactionFix = 0;
    overwriteAttackData->m_reactionAdd = 0;
    overwriteAttackData->m_power = 0;
    overwriteAttackData->m_vector = 90;
    overwriteAttackData->m_size = size;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = 0;

    overwriteAttackData->m_nodeIndex = 0x1;

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_FIGHTER | COLLISION_CATEGORY_MASK_GIMMICK;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = true;
    overwriteAttackData->m_targetSituationGround = true;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_Magic;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_None;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Off;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = false;
    overwriteAttackData->m_isReflectable = false;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_serialHitFrame = 0x3c;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = true;
    overwriteAttackData->m_isInvalidXlu = true;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = true;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = true;
    overwriteAttackData->m_subShield = 0;

    overwriteAttackData->m_shapeType = soCollision::Shape_Sphere;
}

void grQbertGreen::setStart() {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    this->timer = randf()*(qbertStageData->greenMaxRespawnFrames - qbertStageData->greenMinRespawnFrames) + qbertStageData->greenMinRespawnFrames;
    this->m_yakumono->setTeam(15);
    this->setMotion(0);
    grQbertEnemy::setStart();
}

void grQbertGreen::updateMove(float frameDelta) {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    float animFrames = this->m_modelAnims[0]->getFrame();
    float animFrameCount = this->m_modelAnims[0]->getFrameCount();
    float jumpCompletion = animFrames / animFrameCount;

    if (this->timer > 0) { // Respawning
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
        if (this->targetPos.m_y <= this->stage->m_deadRange.m_down) {
            this->setStart();
        }
        else {
            Vec3f pos = this->targetPos + this->shakeOffset;
            this->setPos(&pos);
            this->soundGenerator.playSE(snd_se_stage_Madein_07, 0x0, 0x0, 0xffffffff);
        }
    }
    else if (animFrames - animFrameCount > qbertStageData->jumpWaitFrames) { // Pick new target
        this->setTargetPos();
    }
    else {
        Vec3f pos = this->targetPos + this->shakeOffset;
        this->setPos(&pos);
    }
}

void grQbertGreen::onInflictEach(soCollisionLog* collisionLog, float power) {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    int entryId = g_ftManager->getEntryIdFromTaskId(collisionLog->m_taskId, NULL);
    if (entryId != -1) {
        int team = g_ftManager->getTeam(entryId, true, true);
        g_ftManager->setSlow(team, true, 10, qbertStageData->immobilizeFrames);
        *this->immobilizeStateWork = Immobilize_All;
        this->setStart();
        team = g_ftManager->getTeam(entryId, false, false);
        if (team < NUM_PLAYERS) {
            this->teamScoresWork[team] += GREEN_POINTS;
            if (this->gameRule == Game_Rule_Coin) {
                g_ftManager->pickupCoin(entryId, GREEN_POINTS);
            }
        }
    }
    else {
        // Check if QBert touched the orb
        if (strcmp(gfTask::getTask(collisionLog->m_taskId)->m_taskName, "ykNormal") == 0) {
            g_ftManager->setSlow(-1, true, 5, qbertStageData->immobilizeFrames);
            *this->immobilizeStateWork = Immobilize_Enemies;
            this->setStart();
        }
    }
}

void grQbertGreen::setImmobilizeStateWork(ImmobilizeState* immobilizeStateWork) {
    this->immobilizeStateWork = immobilizeStateWork;
}