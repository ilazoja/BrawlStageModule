#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_phendrana_pinch.h"
#include <sc/sc_melee.h>
#include <ft/ft_manager.h>
#include <gm/gm_global_mode_melee.h>
#include <OS/OSError.h>

grPhendranaPinch* grPhendranaPinch::create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode)
{
    grPhendranaPinch* phendranaPinch = new (Heaps::StageInstance) grPhendranaPinch(taskName);
    phendranaPinch->setup(mdlIndex, taskName, stage, isRidleyNode);

    return phendranaPinch;
}

void grPhendranaPinch::startup(gfArchive* archive, u32 unk1, u32 unk2)
{
    grPhendranaItem::startup(archive, unk1, unk2);

    this->setMotion(0);
    this->setMotionRatio(0);

    this->setNodeVisibilityAll(false, 0);
}

void grPhendranaPinch::update(float deltaFrame)
{
    grPhendranaItem::update(deltaFrame);
    
    if (!this->isActivated && this->checkForPinch() && (!this->isRidleyNode || !this->checkForFighterRidley())) {
        this->activatePinch();
    }
    if (this->isActivated && !this->isFinished) {
        if (this->m_gimmickMotionPath != NULL) {
            if (this->m_gimmickMotionPath->isEndFrame()) {
                this->setNodeVisibilityAll(false, 0);
                this->isFinished = true;
            }
        }
    }
}

void grPhendranaPinch::setMotionPathData(int mdlIndex, bool isRotateMotionPath, u8 effectIndex, int soundEffectIndex)
{
    this->motionPathData.set(1.0, 0, grGimmickMotionPathData::Path_Once, mdlIndex, 0);
    this->isRotateMotionPath = isRotateMotionPath;
    this->effectIndex = effectIndex;
    this->soundEffectIndex = soundEffectIndex;
}

void grPhendranaPinch::activatePinch() {
    this->isActivated = true;
    if (this->m_gimmickMotionPath != NULL) {
        this->m_gimmickMotionPath->startMove();
    }
    this->setMotionRatio(1.0);
    this->setNodeVisibilityAll(true, 0);
}

bool grPhendranaPinch::checkForPinch()
{
    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_gameMode == Game_Mode_Training) {
        return false;
    }

    scMelee* scene = static_cast<scMelee*>(gfSceneManager::getInstance()->searchScene("scMelee"));
    stOperatorRuleMelee* operatorRule = static_cast<stOperatorRuleMelee*>(scene->m_operatorRuleGameMode);
    stOperatorDropItem* operatorDropItem = scene->m_operatorDropItem;
    if (!operatorRule->m_isStart) {
        return false;
    }
    if (operatorDropItem != NULL && operatorDropItem->m_isSuddenDeath) {
        return true;
    }
    if (operatorRule->m_isGameSet) {
        return false;
    }
    if (operatorRule->m_remainingFrameTime < 3600 && operatorRule->m_remainingFrameTime > 0) {
        return true;
    }

    int activeFighterCount = 0;
    bool pinchFighterFound = false;
    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1)) {
            if (g_ftManager->getOwner(entryId)->getStockCount() > 0) {
                activeFighterCount++;
                if (activeFighterCount > 2) {
                    return false;
                }
            }
            if (!pinchFighterFound && g_ftManager->getOwner(entryId)->getStockCount() == 1 && (g_ftManager->getOwner(entryId)->getHitPointMax() == 0 || g_ftManager->getOwner(entryId)->getHitPoint() < 100)) {
                pinchFighterFound = true;
            }
        }
    }

    return pinchFighterFound;
}