#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_phendrana_other.h"
#include <OS/OSError.h>

grPhendranaOther* grPhendranaOther::create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode)
{
    grPhendranaOther* phendranaOther = new (Heaps::StageInstance) grPhendranaOther(taskName);
    phendranaOther->setup(mdlIndex, taskName, stage, isRidleyNode);

    return phendranaOther;
}

void grPhendranaOther::startup(gfArchive* archive, u32 unk1, u32 unk2)
{
    grPhendranaItem::startup(archive, unk1, unk2);

    grGimmickMotionPathInfo motionPathInfo = { archive, &this->motionPathData, this->isRotateMotionPath, true, 0, 0, 0, 0, 0, 0 };
    stTriggerData triggerData = {0,0,1,0};
    this->createAttachMotionPath(&motionPathInfo, &triggerData, "MovePlatformNode");
    this->setMotion(0);

    if (this->effectIndex != 0xFF) {
        this->createEffectWork(1);
        SimpleEffectData simpleEffectData;
        this->createSimpleEffectData(&simpleEffectData, (EfID)(0x320000 + this->effectIndex), "Effect");
        if (0 < simpleEffectData.m_id) {
            this->m_effects[0].m_id = simpleEffectData.m_id;
            this->m_effects[0].m_repeatFrame = simpleEffectData.m_repeatFrame;
            this->m_effects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
            this->m_effects[0].m_endFrame = simpleEffectData.m_endFrame;
            this->m_effects[0].m_offsetPos = (Vec2f){0.0, 0.0};
            this->m_effects[0].m_scale = 1.0;
        }
    
        this->startGimmickEffect(0);
    }
}