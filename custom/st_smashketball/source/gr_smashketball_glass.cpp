#include "gr_smashketball_glass.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include "st_smashketball_data.h"

grSmashketballGlass* grSmashketballGlass::create(int mdlIndex, char* tgtNodeName, char* taskName){
    grSmashketballGlass* glass = new(Heaps::StageInstance) grSmashketballGlass(taskName);
    glass->setupMelee();
    glass->setMdlIndex(mdlIndex);
    glass->m_heapType = Heaps::StageInstance;
    glass->makeCalcuCallback(1, Heaps::StageInstance);
    glass->setCalcuCallbackRoot(7);

    return glass;
}

void grSmashketballGlass::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    this->setupHitPoint();

    grMadein::startup(archive, unk1, unk2);

    this->createSoundWork(2,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Madein_02;
    this->m_soundEffects[0].m_0x10 = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_0x14 = 0;
    this->m_soundEffects[0].m_0x1c = 0.0;
    this->m_soundEffects[0].m_0x20 = 0.0;

    this->m_soundEffects[1].m_id = snd_se_stage_Madein_03;
    this->m_soundEffects[1].m_0x10 = 0;
    this->m_soundEffects[1].m_nodeIndex = 0;
    this->m_soundEffects[1].m_0x14 = 0;
    this->m_soundEffects[1].m_0x1c = 0.0;
    this->m_soundEffects[1].m_0x20 = 0.0;

    this->initializeEntity();
    this->startEntity();

    this->setHit();
}

void grSmashketballGlass::setupHitPoint() {
    Vec3f startOffsetPos = {-5,0,0};
    Vec3f endOffsetPos = {5,0,0};
    this->setHitPoint(7.0, &startOffsetPos, &endOffsetPos, 1, 1);
}

void grSmashketballGlass::setHit() {
    // Done cause Brawl devs allocated ykData on the stack in grMadein::setupYakumonoClass leading to ykData being a garbage pointer so have to replace it in order to be able to change the HitSelfCategory
    this->m_yakumono->m_data = &this->yakumonoData;
    this->setSituationODD();
}

void grSmashketballGlass::update(float deltaFrame)
{
    if (this->timer > 0) {
        this->timer -= deltaFrame;
        if (timer <= 0) {
            this->setMotion(0);
            this->setSleepHit(false);
            this->setEnableCollisionStatus(true);
            this->startGimmickSE(1);
        }
    }
}

void grSmashketballGlass::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stSmashketballData* smashketballData = (stSmashketballData*)this->getStageData();
    this->setSleepHit(true);
    this->setEnableCollisionStatus(false);
    this->setMotion(1);
    this->timer = smashketballData->glassRespawnFrames;
    this->startGimmickSE(0);
}
