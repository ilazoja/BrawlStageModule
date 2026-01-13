#include "st_finalsub.h"
#include "gr_final.h"
#include <memory.h>
#include <st/st_class_info.h>
#include <cm/cm_camera_controller.h>

static stClassInfoImpl<Stages::Final, stFinalSub> classInfo = stClassInfoImpl<Stages::Final, stFinalSub>();

stFinalSub* stFinalSub::create()
{
    stFinalSub* stage = new (Heaps::StageInstance) stFinalSub();
    return stage;
}
bool stFinalSub::loading()
{
    return true;
}
void stFinalSub::update(float deltaFrame)
{
    return;
}

void stFinalSub::createObj()
{
    testStageParamInit(m_fileData, 0xA);
    testStageDataInit(m_fileData, 0x14, 1);
    grFinal* ground = grFinal::create(1, "", "grFinalMainBg");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setType(0);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
    ground = grFinal::create(2, "", "grFinalStage");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setType(1);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
    ground = grFinal::create(3, "", "grFinalSub");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setType(2);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
       //ground->setDontMoveGround();
    }
    createCollision(m_fileData, 2, NULL);
    initCameraParam();
    nw4r::g3d::ResFile posData(m_fileData->getData(Data_Type_Model, 0x64, 0xfffe));
    if (posData.ptr() == NULL)
    {
        // if no stgPos model in pac, use defaults
        createStagePositions();
    }
    else
    {
        createStagePositions(&posData);
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x1E);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);

}


bool stFinalSub::isBamperVector()
{
    return true;
}
GXColor stFinalSub::getFinalTechniqColor()
{
    return (GXColor){0x14000496};
}

ST_CLASS_INFO;