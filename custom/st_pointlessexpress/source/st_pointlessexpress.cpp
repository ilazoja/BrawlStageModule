#include <memory.h>
#include <st/st_class_info.h>
#include "st_pointlessexpress.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stPointlessExpress> classInfo = stClassInfoImpl<Stages::Final, stPointlessExpress>();

stPointlessExpress* stPointlessExpress::create() {
    stPointlessExpress* stage = new(Heaps::StageInstance) stPointlessExpress(Stages::Final);
    return stage;
}

bool stPointlessExpress::loading(){
    return true;
}

void stPointlessExpress::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjGround(1);
    createObjSky(2);
    //createObjFire(1);
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

void stPointlessExpress::createObjGround(int mdlIndex) {
    grFinal* ground = grFinal::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stPointlessExpress::createObjSky(int mdlIndex) {
    grFinal* ground = grFinal::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();

        u32 firesIndex = ground->getNodeIndex(0, "Fires");
        u32 endIndex = ground->getNodeIndex(0, "End");
        for (int i = firesIndex + 1; i < endIndex; i++) {
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(i).ptr();
            this->createObjFire(resNodeData->m_rotation.m_x, resNodeData->m_translation.xy(),
                                resNodeData->m_rotation.m_z, resNodeData->m_scale.m_z, resNodeData->m_translation.m_z, resNodeData->m_scale.m_x, resNodeData->m_rotation.m_y, resNodeData->m_scale.m_y);
        }
    }
}

void stPointlessExpress::createObjFire(int mdlIndex, Vec2f* pos, float rot, float scale, int motionPathIndex, float size, int vector, bool isCapsule) {
    grFire* lava = grFire::create(mdlIndex, "", "grLava");
    if (lava != NULL)
    {
        addGround(lava);
        lava->setMotionPathData(motionPathIndex);
        lava->setSizeAndVector(size, vector, isCapsule);
        lava->setStageData(m_stageData);
        lava->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        lava->initializeEntity();
        lava->startEntity();
        lava->setPos(pos->m_x, pos->m_y, 0.0);
        lava->setRot(0.0, 0.0, rot);
        lava->setScale(scale, scale, scale);
    }
}

void stPointlessExpress::update(float frameDelta){

}

bool stPointlessExpress::isBamperVector() {
   return true;
}
GXColor stPointlessExpress::getFinalTechniqColor() {
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