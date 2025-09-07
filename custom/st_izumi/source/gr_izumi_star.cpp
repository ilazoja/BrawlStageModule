#include "st_izumi_data.h"
#include "gr_izumi_star.h"
#include <ec/ec_mgr.h>
#include <mt/mt_prng.h>
#include <memory.h>
#include <stdio.h>
#include <OS/OSError.h>

grIzumiStar* grIzumiStar::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grIzumiStar* ground = new (Heaps::StageInstance) grIzumiStar(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    ground->setupMelee();
    return ground;
}

void grIzumiStar::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grMadein::startup(archive, unk1, unk2);

    this->createEffectWork(2);

    SimpleEffectData simpleEffectData;
    this->createSimpleEffectData(&simpleEffectData, (EfID)0x330003, "StarFallNode");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[0].m_id = simpleEffectData.m_id;
        this->m_effects[0].m_repeatFrame = simpleEffectData.m_repeatFrame;
        if (simpleEffectData.m_nodeIndex == 0) {
            this->m_effects[0].m_nodeIndex = this->getNodeIndex(0, "StarFallNode");
        }
        else {
            this->m_effects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
        }
        this->m_effects[0].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[0].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[0].m_scale = 1.0;
    }

    this->createSimpleEffectData(&simpleEffectData, (EfID)0x330004, "StarFallNode");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[1].m_id = simpleEffectData.m_id;
        this->m_effects[1].m_repeatFrame = simpleEffectData.m_repeatFrame;
        if (simpleEffectData.m_nodeIndex == 0) {
            this->m_effects[1].m_nodeIndex = this->getNodeIndex(0, "StarFallNode");
        }
        else {
            this->m_effects[1].m_nodeIndex = simpleEffectData.m_nodeIndex;
        }
        this->m_effects[1].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[1].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[1].m_scale = 1.0;
    }

    this->createSoundWork(2,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Izumi_Tangible_Star_Entry;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[1].m_id = snd_se_stage_Izumi_Tangible_Star_Exit;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = 0;
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);

    this->setTimer();
}

void grIzumiStar::update(float deltaFrame)
{
    grMadein::update(deltaFrame);

    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    this->timer -= deltaFrame;
    if (this->timer <= 0) {
        this->startStarEffect(randi(NUM_STAR_COLOURS));
        this->setTimer();
        if (this->lastPathIndex < 0) {
            this->lastPathIndex = randi(this->m_resFile.GetResAnmChrNumEntries() - 1);
            this->setMotion(this->lastPathIndex + 1);
        }
        else {
            int pickedPathIndex = randi(this->m_resFile.GetResAnmChrNumEntries() - 2);
            if (pickedPathIndex >= this->lastPathIndex) {
                pickedPathIndex += 1;
            }
            this->lastPathIndex = pickedPathIndex;
            this->setMotion(this->lastPathIndex + 1);
        }
    }
}

void grIzumiStar::setTimer()
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());
    this->timer = randi(izumiData->starFallMaxFrames - izumiData->starFallMinFrames) + izumiData->starFallMinFrames;
}

void grIzumiStar::startStarEffect(u32 colourIndex) {
    this->stopStarEffect();
    this->startGimmickEffect(0);
    g_ecMgr->preSetAnmIdx(colourIndex, colourIndex, colourIndex, colourIndex, colourIndex, colourIndex);
    this->updataGimmickEffect(0);
    this->startGimmickEffect(1);
}

void grIzumiStar::stopStarEffect() {
    this->stopGimmickEffect(0);
    this->stopGimmickEffect(1);
}