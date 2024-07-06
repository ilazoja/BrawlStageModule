#include "st_izumi_data.h"
#include "gr_izumi_star_item.h"
#include <ec/ec_mgr.h>
#include <mt/mt_prng.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <stdio.h>
#include <OS/OSError.h>
#include <so/so_value_accesser.h>
#include <so/ground/so_ground_util.h>
#include <st/st_utility.h>
#include <it/it_manager.h>

grIzumiStarItem* grIzumiStarItem::create(int mdlIndex, const char* tgtNodeName, const char* taskName, stMelee* stage)
{
    grIzumiStarItem* ground = new (Heaps::StageInstance) grIzumiStarItem(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    ground->setupMelee();

    ground->stage = stage;

    return ground;
}

void grIzumiStarItem::update(float deltaFrame)
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    if (this->state == State_Vanish && g_GameGlobal->m_modeMelee->m_meleeInitData.m_itemFrequency > 0 && g_GameGlobal->m_modeMelee->m_meleeInitData.m_itSwitch.m_item.ex.m_stage) {
        this->timer -= deltaFrame;
        if (this->timer <= 0) {
            itManager* itemManager = itManager::getInstance();
            itGenSheetKind sheetKind = itemManager->getRandBasicItemSheet((itGenKind)STAR_IT_GEN_KIND);
            itManager::ItemSwitch itemSwitch(true);
            ItemKind itemKind = itemManager->getLotOneItemKind(&sheetKind, (itGenKind)STAR_IT_GEN_KIND, &itemSwitch, false);

            BaseItem* item = itemManager->createItem(itemKind.m_kind, itemKind.m_variation);
            if (item != NULL) {

                float midPosX = (this->stage->m_deadRange.m_left + this->stage->m_deadRange.m_right)/2;
                float startPosX = randf()*(izumiData->starItemPosRange) + midPosX - izumiData->starItemPosRange/2;
                Vec3f startPos = (Vec3f){startPosX, this->stage->m_deadRange.m_top, 0.0};
                this->setPos(&startPos);
                this->speedX = randf()*(izumiData->starItemSpeedXRange) - izumiData->starItemSpeedXRange/2;

                switch (itemKind.m_kind) {
                    case Item_Star:
                        this->startStarEffect(1);
                        break;
                    case Item_WarpStar:
                        this->startStarEffect(2);
                        break;
                    case Item_StarRod:
                        this->startStarEffect(3);
                        break;
                    case Item_Dragoon_Part:
                        this->startStarEffect(4);
                        break;
                    default:
                        this->startStarEffect(0);
                        break;
                }

                this->state = State_Falling;

                item->warp(&startPos);
                item->setVanishMode(false);
                item->setVisibilityWhole(false);

                this->itemInstanceId = item->m_instanceId;
            }
        }
    }
    else if (this->state == State_Falling) {
        Vec3f dir = (Vec3f){this->speedX*deltaFrame, izumiData->starItemSpeedY*deltaFrame, 0.0};
        Vec3f newPos = this->getPos() + dir;
        this->setPos(&newPos);

        Vec3f outHitPos;
        Vec3f outCollNormalVec;

        itManager* itemManager = itManager::getInstance();
        BaseItem* item = itemManager->getItemFromInstanceId(this->itemInstanceId);

        if (item == NULL) {
            this->stopStarEffect();
            this->setTimer();
            this->state = State_Vanish;
        }
        else {
            if (item->isHaved()) {
                this->stopStarEffect();
                this->setTimer();
                this->state = State_Vanish;
            }
            else if (soGroundUtil::checkDeadArea(&newPos, 0.0, 0.0) > 0) {
                this->stopStarEffect();
                this->setTimer();
                this->state = State_Vanish;
                item->setVanishMode(true);
            }
            else if (stRayCheck(&newPos, &dir, &outHitPos, &outCollNormalVec, 1, NULL, 0, 5)) {
                this->stopStarEffect();
                this->setTimer();
                this->state = State_Vanish;
                g_ecMgr->setEffect(0x37, &outHitPos);

                float lr = 1.0;
                if (this->speedX < 0) {
                    lr = -1.0;
                }

                newPos += outCollNormalVec*5;

                item->setSafePos(&newPos.m_xy);
                item->reset(&newPos, lr, 0);
                item->setVisibilityWhole(true);
            }
            else {
                item->warp(&newPos);
            }
        }


        // TODO: Twinkle sfx?
    }


}

void grIzumiStarItem::setTimer() {
    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_itemFrequency > 0) {
        stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());
        stIzumiData::ItemDrop starItemDropFrames = izumiData->starItemDropFrames[g_GameGlobal->m_modeMelee->m_meleeInitData.m_itemFrequency - 1];
        this->timer = randi(starItemDropFrames.max - starItemDropFrames.min) + starItemDropFrames.min;
    }

}
