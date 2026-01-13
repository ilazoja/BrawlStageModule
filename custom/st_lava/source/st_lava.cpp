#include <memory.h>
#include <st/st_class_info.h>
#include "st_lava.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stLava> classInfo = stClassInfoImpl<Stages::Final, stLava>();

stLava* stLava::create() {
    stLava* stage = new(Heaps::StageInstance) stLava(Stages::Final);
    return stage;
}

bool stLava::loading(){
    return true;
}

void stLava::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjGround(0);
    createObjLava(1);
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

void stLava::createObjGround(int mdlIndex) {
    grFinal* ground = grFinal::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stLava::createObjLava(int mdlIndex) {
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

void stLava::update(float frameDelta){

}

bool stLava::isBamperVector() {
   return true;
}
GXColor stLava::getFinalTechniqColor() {
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