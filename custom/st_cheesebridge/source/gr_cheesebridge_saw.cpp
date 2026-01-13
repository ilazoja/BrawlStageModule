#include "gr_cheesebridge_saw.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <OS/OSError.h>

grCheeseBridgeSaw* grCheeseBridgeSaw::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grCheeseBridgeSaw* saw = new (Heaps::StageInstance) grCheeseBridgeSaw(taskName);
    saw->setMdlIndex(mdlIndex);
    saw->m_heapType = Heaps::StageInstance;
    saw->makeCalcuCallback(1, Heaps::StageInstance);
    saw->setCalcuCallbackRoot(7);
    saw->setupMelee();

    return saw;
}

void grCheeseBridgeSaw::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grCheeseBridgePlatform::startup(archive, unk1, layerType);
    this->m_soundEffects[0].m_id = snd_se_stage_Jungle_01;

    stCheeseBridgeData* stageData = static_cast<stCheeseBridgeData*>(this->getStageData());
    this->railParam = &stageData->sawParam;
    this->respawnTimer = randf()*(railParam->maxRespawnFrames - railParam->minRespawnFrames) + railParam->minRespawnFrames;

    u32 startHitboxNode = this->getNodeIndex(0, "HitboxStart");
    float size = 1.0;

    Vec3f startPos;
    Vec3f endPos;
    this->getNodePosition(&startPos, 0, startHitboxNode);
    this->getNodePosition(&endPos, 0, "HitboxEnd");
    Vec3f offsetPos = endPos - startPos;
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = Attack_Overwrite;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = stageData->reactionEffect;
    overwriteAttackData->m_reactionFix = stageData->reactionFix;
    overwriteAttackData->m_reactionAdd = stageData->reactionAdd;
    overwriteAttackData->m_power = stageData->power;
    overwriteAttackData->m_vector = 90;
    overwriteAttackData->m_size = size;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = stageData->hitstopMultiplier;

    overwriteAttackData->m_nodeIndex = startHitboxNode;

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_ALL;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = true;
    overwriteAttackData->m_targetSituationGround = true;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_Cutup;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Medium;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Cutup;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Off;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = true;
    overwriteAttackData->m_isReflectable = false;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_subShield = stageData->addedShieldDamage;

    overwriteAttackData->m_serialHitFrame = 0x3c;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}

void grCheeseBridgeSaw::setOrientation() {
    if (randi(2) == 0) {
        this->setRot(0, 0, 0);
    }
    else {
        this->setRot(0, 0, 180);
    }
};

