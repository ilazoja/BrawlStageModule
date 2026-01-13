#include <memory.h>
#include <st/st_class_info.h>
#include "st_warfactory.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stWarFactory> classInfo = stClassInfoImpl<Stages::Final, stWarFactory>();

stWarFactory* stWarFactory::create() {
    stWarFactory* stage = new(Heaps::StageInstance) stWarFactory(Stages::Final);
    return stage;
}
bool stWarFactory::loading(){
    return true;
}

void stWarFactory::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjWall(1);
    createObjWall(2);
    createCollision(m_fileData, 2, NULL);
    createObjConveyor(3, 3, 0);
    createObjConveyor(4, 4, 1);
    createObjConveyor(5, 5, 2);
    createObjConveyor(6, 6, 3);

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
    loadStageAttrParam(m_fileData, 30);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);
}

void stWarFactory::createObjWall(int mdlIndex) {
    grWarFactoryWall* ground = grWarFactoryWall::create(mdlIndex, "", "grWarFactoryWall");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stWarFactory::createObjConveyor(int mdlIndex, int collIndex, int conveyorIndex) {
    grWarFactoryConveyor* conveyor = grWarFactoryConveyor::create(mdlIndex, "", "grWarFactoryConveyor");
    if (conveyor != NULL)
    {
        addGround(conveyor);
        ConveyorGimmickData* conveyorGimmickDatas = (ConveyorGimmickData*)this->m_stageData;
        conveyor->setGimmickData(&conveyorGimmickDatas[conveyorIndex]);
        conveyor->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        conveyor->setStageData(m_stageData);
        conveyor->setDontMoveGround();
        createCollision(m_fileData, collIndex, conveyor);
    }
}

void stWarFactory::update(float frameDelta){

}

bool stWarFactory::isBamperVector() {
   return true;
}
GXColor stWarFactory::getFinalTechniqColor() {
   return (GXColor){0x14000496};
}

template<srStageKind I, typename T>
T* stClassInfoImpl<I, T>::create(){
    T* stage = new(Heaps::StageInstance) T(I);
    return stage;
}

template<srStageKind I, typename T>
stClassInfoImpl<I, T>::~stClassInfoImpl() {
   setClassInfo(I,0);
}

template<srStageKind I, typename T>
void stClassInfoImpl<I, T>::preload() {
   return;
}