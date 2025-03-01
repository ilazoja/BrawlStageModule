#include "st_izumi_data.h"
#include "gr_izumi_spout.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mt/mt_prng.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>

grIzumiSpout* grIzumiSpout::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grIzumiSpout* ground = new (Heaps::StageInstance) grIzumiSpout(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grIzumiSpout::update(float deltaFrame)
{
    this->updateLevel(deltaFrame);
    this->updateColl(deltaFrame);
    this->updateEff(deltaFrame);
}

void grIzumiSpout::setMotion(u32 index) {
    if (index != this->currentAnimIndex) {
        this->changeNodeAnim(index, 0);
        this->changeTexSrtAnim(index, 0);
        this->changeVisibleAnim(index, 0);
        this->changeMatColAnim(index, 0);
        this->currentAnimIndex = index;
    }
}

void grIzumiSpout::fountainInit(u32 spoutId)
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    this->spoutId = spoutId;

	this->isActive = true;
    this->startFountainEffect();
    this->sndPtr = this->soundGenerator.playSE(snd_se_stage_Starfox_arwin_eng, 0, 0, -1);

    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_isHazardOff) {
        this->level = Level_Off;
    }
    else {
        this->level = Level_Mid;
        this->spoutTimer = randf()*(izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) + izumiData->stationaryMinFrames;
    }

    this->setMotion(this->level*NUM_SPOUT_LEVELS);

    this->areaData = (soAreaData){ 0, gfArea::Stage_Group_Gimmick_Normal, AREA_SHAPE_FLAG_FOLLOW_NODE, 0, 0, this->getNodeIndex(0, "Trigger"), {0.0, 0.0}, izumiData->areaRange};
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Common,-1);
    trigger->setObserveYakumono(this->m_yakumono);

    this->disableArea();
}

void grIzumiSpout::startFountainEffect()
{
    this->effFrameCount = 0.0;
    g_ecMgr->setDrawPrio(1);
    this->effPtr = g_ecMgr->setEffect((EfID)(0x330001+this->spoutId));
    g_ecMgr->setDrawPrio(0xffffffff);
    g_ecMgr->setParent(this->effPtr, this->m_sceneModels[0], "Splash", 0);
}
void grIzumiSpout::stopFountainEffect()
{
    g_ecMgr->endEffect(this->effPtr);
}

void grIzumiSpout::updateEff(float deltaFrame)
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    Vec3f bonePos;
    this->getNodePosition(&bonePos, 0, "Splash");
    this->soundGenerator.setPos(&bonePos);
    if (bonePos.m_y < izumiData->spoutEffectMinHeight - 0.5 && this->isActive)
    {
        this->stopFountainEffect();
        this->soundGenerator.stopSE(this->sndPtr, 0);
        this->isActive = false;
    }
    else if (bonePos.m_y > izumiData->spoutEffectMinHeight && !this->isActive)
    {
        this->startFountainEffect();
        this->sndPtr = this->soundGenerator.playSE(snd_se_stage_Starfox_arwin_eng, 0, 0, -1);
        this->isActive = true;
    }
    else if (this->isActive) {
        this->effFrameCount += deltaFrame;
        if (this->effFrameCount > izumiData->spoutEffectDuration) {
            this->stopFountainEffect();
            this->startFountainEffect();
            this->effFrameCount = 0;
        }
    }
}

void grIzumiSpout::updateColl(float deltaFrame) {
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    Vec3f bonePos;
    this->getNodePosition(&bonePos, 0, "Platform");
    if (bonePos.m_y >= izumiData->spoutCollisionMinHeight) {
        this->setEnableCollisionStatus(true);
    }
    else {
        this->setEnableCollisionStatus(false);
    }
}

void grIzumiSpout::updateLevel(float deltaFrame) {
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    if (this->level != Level_Off) {
        float currentAnimFrame = this->m_modelAnims[0]->getFrame();
        float animFrameCount = this->m_modelAnims[0]->getFrameCount();

        if (this->spoutTimer > 0) {
            this->spoutTimer -= deltaFrame;
        }
        if (this->spoutTimer <= 0 && (currentAnimFrame >= animFrameCount - 1 || this->currentAnimIndex != WAIT_SINK_RIPPLE_ANIM_INDEX)) {
            u32 numSpouts = 0;
            u32 spouts[NUM_SPOUT_LEVELS - 1];
            for (u32 i = 0; i < NUM_SPOUT_LEVELS; i++) {
                if (this->level != i) {
                    spouts[numSpouts] = i;
                    numSpouts++;
                }
            }

            u32 index = randi(NUM_SPOUT_LEVELS - 1);
            this->setMotion(this->level*NUM_SPOUT_LEVELS + index + 1);
            this->level = (Level)spouts[index];

            if (this->level == Level_Sink) {
                this->spoutTimer = randf()*(izumiData->sinkMaxFrames - izumiData->sinkMinFrames) + izumiData->sinkMinFrames;
                this->enableArea();
            }
            else {
                this->spoutTimer = randf()*(izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) + izumiData->stationaryMinFrames;
                this->disableArea();
            }
        }
        else if (this->spoutTimer < izumiData->riseWarningFrames && this->level == Level_Sink) {
            this->setMotion(WAIT_SINK_RIPPLE_ANIM_INDEX);
        }
        else if (this->currentAnimIndex != this->level*NUM_SPOUT_LEVELS && this->currentAnimIndex != WAIT_SINK_RIPPLE_ANIM_INDEX) {
            if (currentAnimFrame >= animFrameCount - 1) {
                this->setMotion(this->level*NUM_SPOUT_LEVELS);
            }
        }
    }
}

void grIzumiSpout::onGimmickEvent(soGimmickEventInfo* eventInfo, int* taskId)
{
    int entryId = g_ftManager->getEntryIdFromTaskId(*taskId, NULL);
    if (entryId >= 0) {
        stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

        //Fighter *fighter = g_ftManager->getFighter(entryId, -1);
        if (this->spoutTimer < izumiData->cooldownRisingFrames) {
            this->spoutTimer = izumiData->cooldownRisingFrames;
        }
    }
}