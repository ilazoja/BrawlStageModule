#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_krazoa_spawner.h"
#include <mt/mt_prng.h>
#include <it/it_manager.h>
#include <OS/OSError.h>

grKrazoaSpawner* grKrazoaSpawner::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grKrazoaSpawner* ground = new (Heaps::StageInstance) grKrazoaSpawner(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);

    return ground;
}

void grKrazoaSpawner::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grMadein::startup(archive, unk1, unk2);

    grGimmickMotionPathInfo motionPathInfo(archive, &this->motionPathData, false, true);
    stTriggerData triggerData(0,true,stTriggerData::Keep_None);
    this->createAttachMotionPath(&motionPathInfo, &triggerData, "MovePlatformNode");

    this->createSoundWork(1,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Madein_01;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);

    this->timer = randi(this->maxRespawnFrames - this->minRespawnFrames) +  this->minRespawnFrames;
}

void grKrazoaSpawner::update(float deltaFrame)
{
    grMadein::update(deltaFrame);

    itManager *itemManager = itManager::getInstance();
    if (itemManager->getItemNum(Item_Stage_Shellcreeper, 0x0, this->m_taskId, -1) == 0x0) {
        if (this->timer > 0) {
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                if (itemManager->isCompItemKindArchive(Item_Stage_Shellcreeper, 0x0, true)) {
                    this->setMotion(1);
                    if (!this->isSilentDeployment) {
                        this->startGimmickSE(0);
                    }
                    this->isSilentDeployment = false;
                }
                else {
                    this->timer = randi(this->maxRespawnFrames - this->minRespawnFrames) + this->minRespawnFrames;
                }
            }
        }
        else {
            if (this->m_modelAnims[0]->getFrame() >= this->spawnAfterFrames) {
                BaseItem *item = itemManager->createItem(Item_Stage_Shellcreeper, 0x0, this->m_taskId, 0, 0, 0xffff, 0, 0xffff);
                if (item != NULL) {
                    Vec3f spawnPos;
                    this->getNodePosition(&spawnPos, 0, "SpawnPos");
                    item->warp(&spawnPos);
                    item->setVanishMode(false);
                }
                this->timer = randi(this->maxRespawnFrames - this->minRespawnFrames) + this->minRespawnFrames;
            }
        }
    }
}

void grKrazoaSpawner::setConfig(int motionPathMdlIndex, float minRespawnFrames, float maxRespawnFrames, float spawnAfterFrames, bool isFirstSilentDeployment) {
    this->motionPathData.set(1.0, 0, grGimmickMotionPathData::Path_Loop, motionPathMdlIndex, 0);

    this->minRespawnFrames = minRespawnFrames;
    this->maxRespawnFrames = maxRespawnFrames;
    this->spawnAfterFrames = spawnAfterFrames;

    this->isSilentDeployment = isFirstSilentDeployment;
}

