#include <memory.h>
#include <st/st_class_info.h>
#include "st_finkrat.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stFinkrat> classInfo = stClassInfoImpl<Stages::Final, stFinkrat>();

stFinkrat* stFinkrat::create() {
    stFinkrat* stage = new(Heaps::StageInstance) stFinkrat(Stages::Final);
    return stage;
}

bool stFinkrat::loading(){
    return true;
}

void stFinkrat::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    this->createObjGround(0);
    this->createObjLava(1);
    this->createObjElevate(2, 3);
    this->createObjElevate(3, 4);
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
    loadStageAttrParam(m_fileData, 30);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);
}

void stFinkrat::createObjGround(int mdlIndex) {
    grFinal* ground = grFinal::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stFinkrat::createObjLava(int mdlIndex) {
    grLava* lava = grLava::create(mdlIndex, "", "grLava");
    if (lava != NULL)
    {
        addGround(lava);
        lava->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        lava->setStageData(m_stageData);
        lava->setupAttack();
        lava->initializeEntity();
        lava->startEntity();
        lava->setDontMoveGround();
    }
}

void stFinkrat::createObjElevate(int mdlIndex, int collIndex) {
    grElevate* elevate = grElevate::create(mdlIndex, "", "grElevate");
    if (elevate != NULL)
    {
        addGround(elevate);
        elevate->setStageData(m_stageData);
        elevate->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        elevate->initializeEntity();
        elevate->startEntity();
        createCollision(m_fileData, collIndex, elevate);
    }
}

void stFinkrat::update(float frameDelta){

}

bool stFinkrat::isBamperVector() {
   return true;
}
GXColor stFinkrat::getFinalTechniqColor() {
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