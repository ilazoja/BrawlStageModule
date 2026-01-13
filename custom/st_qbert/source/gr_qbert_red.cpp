#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_qbert_red.h"
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

grQbertRed* grQbertRed::create(int mdlIndex, const char* tgtNodeName, const char* taskName, stMelee* stage){
    grQbertRed* ball = new(Heaps::StageInstance) grQbertRed(taskName);
    ball->setMdlIndex(mdlIndex);
    ball->m_heapType = Heaps::StageInstance;
    ball->makeCalcuCallback(1, Heaps::StageInstance);
    ball->setCalcuCallbackRoot(7);
    ball->stage = stage;

    ball->setupAttack();
    ball->setupHitPoint();

    return ball;
}

void grQbertRed::setupAttack() {

    float size = 1.0;
    Vec3f offsetPos = Vec3f(0.0, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = Attack_Overwrite;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = 0x40;
    overwriteAttackData->m_reactionFix = 0;
    overwriteAttackData->m_reactionAdd = 0;
    overwriteAttackData->m_power = 20;
    overwriteAttackData->m_vector = 90;
    overwriteAttackData->m_size = size;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = 1.0;

    overwriteAttackData->m_nodeIndex = 0x1;

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_ALL;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = true;
    overwriteAttackData->m_targetSituationGround = true;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_Normal;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_None;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Thru;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = true;
    overwriteAttackData->m_isReflectable = true;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_serialHitFrame = 120;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_subShield = 0;

    overwriteAttackData->m_shapeType = soCollision::Shape_Sphere;
}

void grQbertRed::setupHitPoint() {
    Vec3f startOffsetPos = Vec3f(0,0,0);
    Vec3f endOffsetPos = Vec3f(0,0,0);
    this->setHitPoint(7.0, &startOffsetPos, &endOffsetPos, 1, 1);
}

void grQbertRed::setStart() {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    this->timer = randf()*(qbertStageData->redMaxRespawnFrames - qbertStageData->redMinRespawnFrames) + qbertStageData->redMinRespawnFrames;
    this->m_yakumono->setTeam(15);
    this->setMotion(0);
    this->damage = 0;
    grQbertEnemy::setStart();
}

void grQbertRed::updateMove(float frameDelta) {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    float animFrames = this->m_modelAnims[0]->getFrame();
    float animFrameCount = this->m_modelAnims[0]->getFrameCount();
    float jumpCompletion = animFrames / animFrameCount;

    if (this->isDead) { // Launched
        this->timer += frameDelta;
        Vec3f pos = this->getPos();
        Rect2D* range = &this->stage->m_deadRange;
        if (pos.m_y <= range->m_down) {
            if (this->timer >= qbertStageData->dropFrames) {
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
                    this->prevPos,
                    this->targetPos,
                    this->targetPos
            };
            float completion = this->timer / qbertStageData->dropFrames;
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

void grQbertRed::onInflictEach(soCollisionLog* collisionLog, float power) {
    this->soundGenerator.playSE(snd_se_stage_Madein_17, 0x0, 0x0, 0xffffffff);
}

void grQbertRed::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    damage->m_damage = 0;
    this->damage += damage->m_damageAdd;
    if (this->damage > qbertStageData->redHP) {
        this->timer = 0;
        this->setSleepAttack(true);
        this->setSleepHit(true);
        this->isDead = true;
        this->angle = damage->m_attackData.m_vector;
        if (damage->m_lr == -1) {
            this->angle = 180 - damage->m_attackData.m_vector;
        }
        this->velocity = damage->m_reaction / 60;

        this->prevPos = this->getPos();
        this->targetPos = Vec3f(this->prevPos.m_x, this->stage->m_deadRange.m_down, 0);

        int teamId = damage->m_collisionLog.m_teamNo;
        if (attackerInfo->m_indirectSoKind == StageObject_Fighter) {
            teamId = g_ftManager->getTeam(attackerInfo->m_indirectEntryId, false, false);
            if (this->gameRule == Game_Rule_Coin) {
                g_ftManager->pickupCoin(attackerInfo->m_indirectEntryId, RED_POINTS);
            }
        }
        if (teamId < NUM_PLAYERS) {
            this->teamScoresWork[teamId] += RED_POINTS;
        }
    }
}