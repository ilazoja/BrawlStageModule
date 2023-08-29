#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_platform.h"
#include <OS/OSError.h>

grPlatform* grPlatform::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grPlatform* ground = new (Heaps::StageInstance) grPlatform(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);

    return ground;
}

void grPlatform::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grMadein::startup(archive, unk1, unk2);

    grGimmickMotionPathInfo motionPathInfo = { archive, &this->motionPathData, 0x01000000, 0, 0, 0, 0, 0, 0 };
    stTriggerData triggerData = {0,0,1,0};
    this->createAttachMotionPath(&motionPathInfo, &triggerData, "MovePlatformNode");
}

void grPlatform::update(float deltaFrame)
{
    grMadein::update(deltaFrame);

    Vec3f pos = (Vec3f){0, 0, 0};
    Vec3f rot = (Vec3f){0, 0, 0};
    Vec3f scale = (Vec3f){0, 0, 0};
    if (this->m_gimmickMotionPath != NULL) {
        this->m_gimmickMotionPath->getTRS(&pos, &rot, &scale);
        this->setRot(&rot);
    }

}



void grPlatform::setMotionPathData(int mdlIndex) {
    this->motionPathData.m_motionRatio = 1.0;
    this->motionPathData.m_index = 0;
    this->motionPathData.m_pathMode = MotionPathMode_Loop;
    this->motionPathData.m_mdlIndex = mdlIndex;
    this->motionPathData._padding = 0x0;
}

