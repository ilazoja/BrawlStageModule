#include "gr_templeoftime.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>
#include <sc/sc_melee.h>

bool grTempleOfTime::checkForPinch() {
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

bool grTempleOfTime::checkForOcarina() {
    bool isOcarinaBeingPlayed = false;
    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1)) {
            Fighter* fighter = g_ftManager->getFighter(entryId, -1);
            if (fighter->getFtKind() == Fighter_Link && fighter->m_moduleAccesser->getStatusModule().getStatusKind() == 279) {
                isOcarinaBeingPlayed = true;
                ipPadButton currentButton = fighter->m_moduleAccesser->getControllerModule().getButton();
                ipPadButton newButton = (ipPadButton){buttonMask.m_mask & currentButton.m_mask & ~prevButton.m_mask};
                if (newButton.m_mask > 0) {
                    if (this->ocarinaSong->updateAndCheckIfComplete(newButton)) {
                        this->playOcarinaSong();
                        prevButton.m_mask = 0;
                        return true;
                    }
                }

                prevButton.m_mask = currentButton.m_mask;
            }
        }
    }
    if (!isOcarinaBeingPlayed) {
        prevButton.m_mask = 0;
        this->ocarinaSong->reset();
    }
    return false;
}

void grTempleOfTime::playOcarinaSong() {

}
