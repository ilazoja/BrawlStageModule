#include <memory.h>
#include <st/st_class_info.h>
#include "st_cheesebridge.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stCheeseBridge> classInfo = stClassInfoImpl<Stages::Final, stCheeseBridge>();

stCheeseBridge* stCheeseBridge::create() {
    stCheeseBridge* stage = new(Heaps::StageInstance) stCheeseBridge(Stages::Final);
    return stage;
}

bool stCheeseBridge::loading(){
    return true;
}

void stCheeseBridge::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjGround(1);
    createCollision(m_fileData, 2, NULL);

    stCheeseBridgeData* stageData = static_cast<stCheeseBridgeData*>(m_stageData);
    for (int i = 0; i < NUM_RAILS; i++) {
        for (int j = 0; j < stageData->numPlatformsForRails[i]; j++) {
            createObjPlatform(2, 3, 15 + i, i);
        }
    }
    for (int i = 0; i < NUM_RAILS; i++) {
        for (int j = 0; j < stageData->numSawsForRails[i]; j++) {
            createObjSaw(3, 15 + i, i);
        }
    }
    for (int i = 0; i < NUM_RAILS; i++) {
        for (int j = 0; j < stageData->numRopesForRails[i]; j++) {
            createObjRope(4, 15 + i, i);
        }
    }

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

void stCheeseBridge::createObjGround(int mdlIndex) {
    grCheeseBridge* ground = grCheeseBridge::create(mdlIndex, "", "grCheeseBridge");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setStageData(m_stageData);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setDontMoveGround();
    }
}

void stCheeseBridge::createObjPlatform(int mdlIndex, int collIndex, int motionPathIndex, int railIndex) {
    grCheeseBridgePlatform* platform = grCheeseBridgePlatform::create(mdlIndex, "", "grCheeseBridgePlatform");
    if (platform != NULL)
    {
        addGround(platform);
        platform->setMotionPathData(motionPathIndex);
        platform->setCooldownTimerWork(&this->platformCooldownRailTimers[railIndex]);
        platform->setStageData(m_stageData);
        platform->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        createCollision(m_fileData, collIndex, platform);
    }
}

void stCheeseBridge::createObjSaw(int mdlIndex, int motionPathIndex, int railIndex) {
    grCheeseBridgeSaw* saw = grCheeseBridgeSaw::create(mdlIndex, "", "grCheeseBridgeSaw");
    if (saw != NULL)
    {
        addGround(saw);
        saw->setMotionPathData(motionPathIndex);
        saw->setCooldownTimerWork(&this->sawCooldownRailTimers[railIndex]);
        saw->setStageData(m_stageData);
        saw->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        saw->initializeEntity();
        saw->startEntityAutoLoop();
    }
}

void stCheeseBridge::createObjRope(int mdlIndex, int motionPathIndex, int railIndex) {
    grCheeseBridgeRope* rope = grCheeseBridgeRope::create(mdlIndex, "", "grCheeseBridgeRope");
    if (rope != NULL)
    {
        addGround(rope);
        rope->setMotionPathData(motionPathIndex);
        rope->setCooldownTimerWork(&this->ropeCooldownRailTimers[railIndex]);
        rope->setStageData(m_stageData);
        rope->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        rope->initializeEntity();
        rope->startEntityAutoLoop();
    }
}

void stCheeseBridge::update(float frameDelta){
    for (int i = 0; i < NUM_RAILS; i++) {
        if (this->platformCooldownRailTimers[i] > 0) {
            this->platformCooldownRailTimers[i] -= frameDelta;
        }
        if (this->sawCooldownRailTimers[i] > 0) {
            this->sawCooldownRailTimers[i] -= frameDelta;
        }
        if (this->ropeCooldownRailTimers[i] > 0) {
            this->ropeCooldownRailTimers[i] -= frameDelta;
        }
    }
}

bool stCheeseBridge::isBamperVector() {
   return true;
}
GXColor stCheeseBridge::getFinalTechniqColor() {
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