#include <memory.h>
#include <st/st_class_info.h>
#include "st_pictsimple.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>
#include <gm/gm_global.h>
#include <sc/sc_melee.h>
#include <OS/__ppc_eabi_init.h>

static stClassInfoImpl<Stages::PictChat, stPictSimple> classInfo = stClassInfoImpl<Stages::PictChat, stPictSimple>();

stPictSimple* stPictSimple::create() {
    stPictSimple* stage = new(Heaps::StageInstance) stPictSimple(Stages::Final);
    return stage;
}

bool stPictSimple::loading(){
    return true;
}

void stPictSimple::patchInstructions() {
    int *instructionAddr = (int*)0x809582d8;
    *instructionAddr = 0x60000000; // nop Original: bne-
    __flush_cache(instructionAddr - 4, 0x8);
}

void stPictSimple::createObj() {

    this->patchInstructions();
    g_sndSystem->playSE(snd_se_stage_Pictchat_in,-1,0,0,-1);

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjGround(1);
    createObjBg(2);
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

void stPictSimple::createObjGround(int mdlIndex) {
    grPictSimple* ground = grPictSimple::create(mdlIndex, "", "grPictChat");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stPictSimple::createObjBg(int mdlIndex) {
    grPictSimpleBg* ground = grPictSimpleBg::create(mdlIndex, "", "grPictChatBg");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stPictSimple::update(float frameDelta){

}

bool stPictSimple::isBamperVector() {
   return true;
}
GXColor stPictSimple::getFinalTechniqColor() {
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