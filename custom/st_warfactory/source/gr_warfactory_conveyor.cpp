#include "gr_warfactory_conveyor.h"
#include <ec/ec_mgr.h>
#include <memory.h>

grWarFactoryConveyor* grWarFactoryConveyor::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grWarFactoryConveyor* ground = new (Heaps::StageInstance) grWarFactoryConveyor(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grWarFactoryConveyor::startup(gfArchive* archive, u32 unk1, u32 unk2)
{
    grYakumono::startup(archive, unk1, unk2);
    ConveyorGimmickData* conveyorGimmickData = (ConveyorGimmickData*)this->getGimmickData();
    Vec3f pos = Vec3f(conveyorGimmickData->pos.m_x,conveyorGimmickData->pos.m_y,0);
    Vec2f areaPos = Vec2f(0.0, 0.0);
    Vec2f areaRange = Vec2f(conveyorGimmickData->range, 10.0);
    grGimmickBeltConveyorData beltConveyorAreaData(
            &pos,
            conveyorGimmickData->speed,
            conveyorGimmickData->isRightDirection,
            &areaPos,
            &areaRange,
            gfArea::Shape_Rectangle
    );

    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor,-1);
    trigger->setBeltConveyorTrigger(&beltConveyorAreaData);
}

void grWarFactoryConveyor::update(float deltaFrame)
{

}
