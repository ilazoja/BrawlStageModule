#include "st_izumi_data.h"
#include "gr_izumi.h"
#include <ec/ec_mgr.h>
#include <memory.h>

grIzumi* grIzumi::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grIzumi* ground = new (Heaps::StageInstance) grIzumi(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grIzumi::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
    u32 nodeIndex = -1;
    if (this->getNodeIndex(&nodeIndex, 0, "sound_center")) {
        Vec3f pos;
        this->getNodePosition(&pos, 0, nodeIndex);
        this->centerSoundGenerator.setPos(&pos);
        this->centerSoundGenerator.playSE(snd_se_stage_Izumi_Ambience, 0, 0, -1);
    }

}

void grIzumi::update(float deltaFrame)
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());
    float animFrames = this->m_modelAnims[0]->getFrame();
    u32 nodeIndex;

    if (animFrames == izumiData->ringSndStartFrame[0]) {
        if (this->getNodeIndex(&nodeIndex, 0, "sound_ring1")) {
            Vec3f pos;
            this->getNodePosition(&pos, 0, nodeIndex);
            this->ringSoundGenerators[0].stopSE(this->ringSndPtr[0], 0);
            this->ringSoundGenerators[0].setPos(&pos);
            this->ringSoundGenerators[0].playSE(snd_se_stage_Izumi_Ring_Left, 0, 0, -1);
        }
    }
    if (animFrames == izumiData->ringSndStartFrame[1]) {
        if (this->getNodeIndex(&nodeIndex, 0, "sound_ring2")) {
            Vec3f pos;
            this->getNodePosition(&pos, 0, nodeIndex);
            this->ringSoundGenerators[1].stopSE(this->ringSndPtr[1], 0);
            this->ringSoundGenerators[1].setPos(&pos);
            this->ringSoundGenerators[1].playSE(snd_se_stage_Izumi_Ring_Right, 0, 0, -1);
        }
    }

}
