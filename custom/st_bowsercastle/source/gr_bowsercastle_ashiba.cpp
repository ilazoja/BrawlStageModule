#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_bowsercastle_ashiba.h"
#include <OS/OSError.h>

grBowserCastleAshiba* grBowserCastleAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grBowserCastleAshiba* bg = new(Heaps::StageInstance) grBowserCastleAshiba(taskName);
    bg->setupMelee();
    bg->setMdlIndex(mdlIndex);
    bg->setTgtNode(tgtNodeName);

    return bg;
}

void grBowserCastleAshiba::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);

    this->createSoundWork(1,1);
    this->m_soundEffects[0].m_id = snd_se_stage_BowserCastle_Ambience;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = this->getNodeIndex(0, "LavaSfx");
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);

    this->startGimmickSE(0);
}

//void grBowserCastleAshiba::update(float frameDiff){
//
//}
