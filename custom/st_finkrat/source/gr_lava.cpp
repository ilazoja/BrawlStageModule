#include "gr_lava.h"
#include <ec/ec_mgr.h>
#include <memory.h>

grLava* grLava::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grLava* lava = new (Heaps::StageInstance) grLava(taskName);
    lava->setMdlIndex(mdlIndex);
    lava->m_heapType = Heaps::StageInstance;
    lava->makeCalcuCallback(1, Heaps::StageInstance);
    lava->setCalcuCallbackRoot(7);
    lava->setupMelee();

    return lava;
}

void grLava::update(float deltaFrame)
{

}

void grLava::setupAttack() {
    grFinkratData* finkratData = static_cast<grFinkratData*>(this->getStageData());

    float size = 1.0;
    Vec3f offsetPos = Vec3f(finkratData->lavaWidth, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = finkratData->lavaReactionEffect;
    overwriteAttackData->m_reactionFix = finkratData->lavaReactionFix;
    overwriteAttackData->m_reactionAdd = finkratData->lavaReactionAdd;
    overwriteAttackData->m_power = finkratData->lavaPower;
    overwriteAttackData->m_vector = 90;
    overwriteAttackData->m_size = size;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = finkratData->lavaHitstopMultiplier;

    overwriteAttackData->m_nodeIndex = 0x1;

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_ALL;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = true;
    overwriteAttackData->m_targetSituationGround = true;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_Fire;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Fire;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Off;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = false;
    overwriteAttackData->m_isReflectable = false;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_serialHitFrame = finkratData->lavaDetectionRate;
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

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}
