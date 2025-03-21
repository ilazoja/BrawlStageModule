#include "gr_templeoftime.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>
#include <sc/sc_melee.h>

bool grTempleOfTime::checkForPinch() {
    scMelee* scene = static_cast<scMelee*>(gfSceneManager::getInstance()->searchScene("scMelee"));
    stOperatorRuleMelee* operatorRule = static_cast<stOperatorRuleMelee*>(scene->m_operatorRuleGameMode);
    if (operatorRule->m_remainingFrameTime < 3600) {
        return true;
    }

    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1)) {
            if (g_ftManager->getOwner(entryId)->getStockCount() == 1) {
                return true;
            }
        }
    }

    return false;
}

bool grTempleOfTime::checkForOcarina() {
    bool isOcarinaBeingPlayed = false;
    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1)) {
            Fighter* fighter = g_ftManager->getFighter(entryId, -1);
            if (fighter->getFtKind() == Fighter_Link && fighter->m_moduleAccesser->getStatusModule()->getStatusKind() == 279) {
                isOcarinaBeingPlayed = true;
                ipPadButton currentButton = fighter->m_moduleAccesser->getControllerModule()->getButton();
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
