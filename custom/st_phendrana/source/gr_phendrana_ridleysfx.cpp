#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_phendrana_ridleysfx.h"
#include <sc/sc_melee.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>

grPhendranaRidleySfx* grPhendranaRidleySfx::create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode)
{
    grPhendranaRidleySfx* phendranaRidleySfx = new (Heaps::StageInstance) grPhendranaRidleySfx(taskName);
    phendranaRidleySfx->setup(mdlIndex, taskName, stage, isRidleyNode);

    return phendranaRidleySfx;
}

void grPhendranaRidleySfx::startup(gfArchive* archive, u32 unk1, u32 unk2)
{
    grPhendranaPinch::startup(archive, unk1, unk2);

    this->createSoundWork(2, 2);
    
    this->m_soundEffects[0].m_id = snd_se_stage_Pirates_17;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = this->getNodeIndex(0, "RidleySfx_WingSfx");
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = (Vec2f){0.0, 0.0};
    this->m_soundEffects[0].m_generatorIndex = 0;

    this->m_soundEffects[1].m_id = snd_se_stage_Pirates_16;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = this->getNodeIndex(0, "RidleySfx_RoarSfx");
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = (Vec2f){0.0, 0.0};
    this->m_soundEffects[1].m_generatorIndex = 1;
}

void grPhendranaRidleySfx::update(float deltaFrame)
{
    grPhendranaPinch::update(deltaFrame);
    
    if (this->isActivated && !this->isFinished) {
        this->currentLoop = int(this->getMotionFrame(0) / 152);
        if ((int)this->getMotionFrame(0) == 152 * currentLoop + 42) {
            this->startGimmickSE(0);
        }
        if (this->getMotionFrame(0) == 880) {
            this->startGimmickSE(1);
        }
        if (this->getMotionFrame(0) >= 2199) {
            this->isFinished = true;
        }
    }
}

void grPhendranaRidleySfx::activatePinch()
{
    grPhendranaPinch::activatePinch();
}