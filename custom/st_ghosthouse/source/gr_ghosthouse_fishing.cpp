#include "gr_ghosthouse_fishing.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <math.h>
#include <hkmath/hkMath.h>
#include <it/it_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>

grGhostHouseFishing* grGhostHouseFishing::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grGhostHouseFishing* boo = new (Heaps::StageInstance) grGhostHouseFishing(taskName);
    boo->setMdlIndex(mdlIndex);
    boo->m_heapType = Heaps::StageInstance;
    boo->makeCalcuCallback(1, Heaps::StageInstance);
    boo->setCalcuCallbackRoot(7);
    boo->setupMelee();

    return boo;
}

void grGhostHouseFishing::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grMadein::startup(archive, unk1, unk2);

    this->createEffectWork(1);
    SimpleEffectData simpleEffectData;
    this->createSimpleEffectData(&simpleEffectData, (EfID)0x320004, "Effect");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[0].m_id = simpleEffectData.m_id;
        this->m_effects[0].m_repeatFrame = simpleEffectData.m_repeatFrame;
        this->m_effects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
        this->m_effects[0].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[0].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[0].m_scale = 1.0;
    }
}

void grGhostHouseFishing::setupAttack() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    float size = 1.0;
    Vec3f offsetPos = Vec3f(0.0, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = ghostHouseData->booReactionEffect;
    overwriteAttackData->m_reactionFix = ghostHouseData->booReactionFix;
    overwriteAttackData->m_reactionAdd = ghostHouseData->booReactionAdd;
    overwriteAttackData->m_power = ghostHouseData->booPower;
    overwriteAttackData->m_vector = ghostHouseData->booVector;
    overwriteAttackData->m_size = 1.0;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = ghostHouseData->booHitstopMultiplier;

    overwriteAttackData->m_nodeIndex = this->getNodeIndex(0, "Attack");

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_ALL;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = true;
    overwriteAttackData->m_targetSituationGround = true;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_Normal;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Punch;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Off;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = true;
    overwriteAttackData->m_isReflectable = false;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_serialHitFrame = ghostHouseData->booDetectionRate;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_subShield = ghostHouseData->booShieldDamage;

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}

void grGhostHouseFishing::update(float deltaFrame) {
    grMadein::update(deltaFrame);
    this->updateState(deltaFrame);
}

void grGhostHouseFishing::updateState(float deltaFrame) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());
    switch(this->state) {
        case State_Bait:
        {
            Vec2f targetPos = (*this->northEastPos + *this->southWestPos) / 2;
            this->moveToTarget(&targetPos, &ghostHouseData->fishingBaitMaxSpeed, &ghostHouseData->fishingBaitAccel, deltaFrame);
            itManager* itemManager = itManager::getInstance();
            BaseItem* item = itemManager->getItemFromInstanceId(this->itemInstanceId);

            if (item == NULL) {
                this->changeState(State_Fetch);
            }
            else if (item->isHaved()) {
                int teamOwnerTaskId = item->getTeamOwnerTaskId();
                int teamOwnerEntryId = g_ftManager->getEntryIdFromTaskId(teamOwnerTaskId, NULL);
                this->setStalk(teamOwnerEntryId);
            }
            else {
        //            else if (u32 statusKind = soExternalValueAccesser::getStatusKind(item), statusKind == 7 || statusKind == 6) {
//                this->changeState(State_Fetch);
//

                this->moveItem(item);
                this->timer -= deltaFrame;
                if (this->timer <= 0) {
                    item->changeStatus(6);
                }
                else if (this->timer <= item->getParam(itValueAccesser::Game_Param_Int_Remove_Flash_Frame)) {
                    item->setVisibilityWhole(static_cast<int>(this->timer) % FISHING_ITEM_FLICKER_RATE != 0);
                }
            }

        }
            break;
        case State_Stalk:
            if (g_ftManager->isFighterActivate(this->targetEntryId, -1)) {
                Vec3f targetPos = g_ftManager->getFighterCenterPos(this->targetEntryId, -1);
                targetPos.m_y = (this->northEastPos->m_y + this->southWestPos->m_y) / 2;
                this->moveToTarget(targetPos.xy(), &ghostHouseData->fishingStalkMaxSpeed, &ghostHouseData->fishingStalkAccel, deltaFrame);

                if (this->timer > 0) {
                    this->timer -= deltaFrame;
                    if (this->timer <= 0) {
                        this->startGimmickEffect(0);
                        this->setSleepAttack(false);
                    }
                }
            }
            else {
                this->changeState(State_Fetch);
            }

            break;
        case State_Fetch:
        {
            Vec3f pos = this->getPos();
            if (pos.m_x >= this->northEastPos->m_x || pos.m_x <= this->southWestPos->m_x) {
                this->changeState(State_Bait);
            }
            else {
                Vec3f targetPos = Vec3f(
                        this->speed.m_x >= 0.0 ? this->northEastPos->m_x : this->southWestPos->m_x,
                        (this->northEastPos->m_y + this->southWestPos->m_y) / 2,
                        0.0);
                this->moveToTarget(targetPos.xy(), &ghostHouseData->fishingBaitMaxSpeed, &ghostHouseData->fishingBaitAccel, deltaFrame);
            }
        }

            break;
        default:
            break;
    }
}

void grGhostHouseFishing::moveToTarget(Vec2f* targetPos, Vec2f* maxSpeed, Vec2f* accel, float deltaFrame) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());
    Vec3f currentPos = this->getPos();
    if (currentPos.m_x > targetPos->m_x) {
        this->speed.m_x -= accel->m_x*deltaFrame;
        this->speed.m_x = hkMath::max2<float>(this->speed.m_x, -maxSpeed->m_x);
    }
    else if (currentPos.m_x < targetPos->m_x) {
        this->speed.m_x += accel->m_x*deltaFrame;
        this->speed.m_x = hkMath::min2<float>(this->speed.m_x, maxSpeed->m_x);
    }

    if (currentPos.m_y > targetPos->m_y) {
        this->speed.m_y -= accel->m_y*deltaFrame;
        this->speed.m_y = hkMath::max2<float>(this->speed.m_y, -maxSpeed->m_y);
    }
    else if (currentPos.m_y < targetPos->m_y) {
        this->speed.m_y += accel->m_y*deltaFrame;
        this->speed.m_y = hkMath::min2<float>(this->speed.m_y, maxSpeed->m_y);
    }

    Vec2f rotDir;
    if (this->speed.m_x > 0) {
        rotDir = Vec2f(1.0, 0.0);
    }
    else {
        rotDir = Vec2f(-1.0, 0.0);
    }

    this->rotateToDisp(&rotDir, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
    *currentPos.xy() += this->speed;
    this->setPos(&currentPos);
}

void grGhostHouseFishing::moveItem(BaseItem* item) {
    Vec3f pos;
    this->getNodePosition(&pos, 0, "Item");
    item->m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
    item->warp(&pos);
}

void grGhostHouseFishing::setVanish() {
    if (this->state == State_Bait || this->state == State_Stalk) {
        this->changeState(State_Out);
    }
    else {
        this->changeState(State_Vanish);
    }
}

void grGhostHouseFishing::setStart(Vec2f* boundarySWPos, Vec2f* boundaryNEPos) {
    this->southWestPos = boundarySWPos;
    this->northEastPos = boundaryNEPos;

    this->changeState(State_Vanish);
}

void grGhostHouseFishing::setActive(Vec2f* boundarySWPos, Vec2f* boundaryNEPos) {
    this->southWestPos = boundarySWPos;
    this->northEastPos = boundaryNEPos;

    this->changeState(State_Bait);
}

void grGhostHouseFishing::setStalk(int entryId) {
    this->targetEntryId = entryId;
    this->changeState(State_Stalk);
}

void grGhostHouseFishing::changeState(State state) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData *>(this->getStageData());
    if (this->state != state) {
        switch (state) {
            case State_Vanish: {
                this->setNodeVisibilityAll(false, 0);
                this->setSleepAttack(true);
                this->stopGimmickEffect(0);
                Vec3f startPos = Vec3f(
                        randi(2) == 1 ? this->southWestPos->m_x : this->northEastPos->m_x,
                        randf() * (this->northEastPos->m_y - this->southWestPos->m_y) + this->southWestPos->m_y,
                        0.0);
                this->setPos(&startPos);
            }
                break;
            case State_Bait:
            {
                this->speed = Vec2f(0.0, 0.0);
                this->setNodeVisibilityAll(true, 0);
                this->setSleepAttack(true);
                this->stopGimmickEffect(0);
                this->setMotion(0);

                itManager* itemManager = itManager::getInstance();
                itGenSheetKind sheetKind = itemManager->getRandBasicItemSheet((itGenId)(FISHING_IT_GEN_KIND));
                itManager::ItemSwitch itemSwitch(true);
                ItemKind itemKind = itemManager->getLotOneItemKind(&sheetKind, (itGenId)(FISHING_IT_GEN_KIND), &itemSwitch, false);

                BaseItem* item = itemManager->createItem(Item_Heart, itemKind.m_variation);
                if (item == NULL) {
                    this->changeState(State_Fetch);
                }
                else {
                    this->itemInstanceId = item->m_instanceId;
                    Vec3f pos = this->getPos();
                    item->warp(&pos);
                    this->timer = soExternalValueAccesser::getWorkInt(item, BaseItem::Instance_Work_Int_Life_Time);
                }
            }
                break;
            case State_Stalk:
                this->timer = ghostHouseData->fishingFireStartFrames;
                break;
            default:
                break;
        }
        this->state = state;
    }
}

void grGhostHouseFishing::rotateToDisp(Vec2f* disp, float maxRot, float rotateSpeed) {
    Vec3f currentRot = this->getRot();
    float pitch = mtConvRadToDeg(hkMath::acos(fabsf(disp->m_y)/disp->length()));
    pitch = hkMath::min2(maxRot, pitch);
    if (disp->m_x < 0) {
        pitch = -pitch;
    }
    float pitchDiff = pitch - currentRot.m_pitch;
    if (pitchDiff >= 0) {
        pitchDiff = hkMath::min2(pitchDiff, rotateSpeed);
    }
    else {
        pitchDiff = hkMath::max2(pitchDiff, -rotateSpeed);
    }
    currentRot.m_pitch += pitchDiff;

    float roll = mtConvRadToDeg(hkMath::acos(fabsf(disp->m_x)/disp->length()));
    roll = hkMath::min2(maxRot, roll);
    if (disp->m_y >= 0) {
        roll = -roll;
    }
    float rollDiff = roll - currentRot.m_roll;
    if (rollDiff >= 0) {
        rollDiff = hkMath::min2(rollDiff, rotateSpeed);
    }
    else {
        rollDiff = hkMath::max2(rollDiff, -rotateSpeed);
    }
    currentRot.m_roll += rollDiff;

    this->setRot(&currentRot);
}

void grGhostHouseFishing::notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) {
    if (this->state == State_Bait && instanceId == this->itemInstanceId) {
        this->setStalk(entryId);
    }
}