#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_qbert_red.h"
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

    overwriteAttackData->m_masks.category = COLLISION_CATEGORY_MASK_ALL;

    overwriteAttackData->m_bits.isSituationODD = true;
    overwriteAttackData->m_bits.isSituationAir = true;
    overwriteAttackData->m_bits.isSituationGround = true;

    overwriteAttackData->m_bits.field_0x30_3 = false;
    overwriteAttackData->m_masks.part = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_bits.attribute = soCollisionAttackData::Attribute_Normal;

    overwriteAttackData->m_bits.soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_bits.soundAttribute = soCollisionAttackData::Sound_Attribute_None;
    overwriteAttackData->m_bits.isClankable = true;
    overwriteAttackData->m_bits.field_0x34_3 = false;
    overwriteAttackData->m_bits.field_0x34_4 = false;
    overwriteAttackData->m_bits.isBlockable = true;
    overwriteAttackData->m_bits.isReflectable = true;
    overwriteAttackData->m_bits.isAbsorbable = false;
    overwriteAttackData->m_bits.region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_bits.detectionRate = 120;
    overwriteAttackData->m_bits.field_0x38_1 = false;
    overwriteAttackData->m_bits.ignoreInvincibility = false;
    overwriteAttackData->m_bits.ignoreIntangibility = false;
    overwriteAttackData->m_bits.lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_bits.field_0x38_5 = false;
    overwriteAttackData->m_bits.enableFriendlyFire = false;
    overwriteAttackData->m_bits.disableHitstop = false;
    overwriteAttackData->m_bits.disableHitGfx = false;
    overwriteAttackData->m_bits.disableFlinch = false;
    overwriteAttackData->m_bits.addedShieldDamage = 0;

    overwriteAttackData->m_bits.isShapeCapsule = false;
}

void grQbertRed::setupHitPoint() {
    Vec3f startOffsetPos = {0,0,0};
    Vec3f endOffsetPos = {0,0,0};
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
        if (pos.m_y <= range->m_bottom) {
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
        if (this->targetPos.m_y <= this->stage->m_deadRange.m_bottom) {
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
    damage->m_totalDamage = 0;
    this->damage += damage->m_damage;
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
        this->targetPos = (Vec3f){this->prevPos.m_x, this->stage->m_deadRange.m_bottom, 0};

        int teamId = damage->m_collisionLog.m_teamId;
        if (attackerInfo->m_indirectAttackerSoKind == StageObject_Fighter) {
            teamId = g_ftManager->getTeam(attackerInfo->m_indirectAttackerEntryId, false, false);
            if (this->gameRule == Game_Rule_Coin) {
                g_ftManager->pickupCoin(attackerInfo->m_indirectAttackerEntryId, RED_POINTS);
            }
        }
        if (teamId < NUM_PLAYERS) {
            this->teamScoresWork[teamId] += RED_POINTS;
        }
    }
}