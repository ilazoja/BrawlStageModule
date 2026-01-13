#include <memory.h>
#include <st/st_class_info.h>
#include "st_krazoa.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>
#include <gm/gm_global.h>
#include <sc/sc_melee.h>

static stClassInfoImpl<Stages::Final, stKrazoa> classInfo = stClassInfoImpl<Stages::Final, stKrazoa>();

stKrazoa* stKrazoa::create() {
    stKrazoa* stage = new(Heaps::StageInstance) stKrazoa(Stages::Final);
    return stage;
}

bool stKrazoa::loading(){
    return true;
}

void stKrazoa::createObj() {

    int nodeSize;
    void* data = m_fileData->getData(Data_Type_Misc, 0x2711, &nodeSize, 0xfffe);
    if (data != NULL) {
        this->itemBrres.setFileImage(data, nodeSize, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2712, &nodeSize, 0xfffe);
    if (data != NULL) {
        this->itemParam.setFileImage(data, nodeSize, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2713, &nodeSize, 0xfffe);
    if (data != NULL) {
        this->itemCommonParam.setFileImage(data, nodeSize, Heaps::StageResource);
    }

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjGround(0);
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

void stKrazoa::createObjGround(int mdlIndex) {
    grKrazoa* ground = grKrazoa::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();

        u32 laddersIndex = ground->getNodeIndex(0, "Ladders");
        u32 spawnersIndex = ground->getNodeIndex(0, "Spawners");
        u32 endIndex = ground->getNodeIndex(0, "End");

        for (int i = laddersIndex + 1; i < spawnersIndex; i++) {
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(i).ptr();
            this->createObjLadder(resNodeData->m_rotation.m_x, resNodeData->m_translation.xy(), resNodeData->m_translation.m_z,
                                  resNodeData->m_rotation.m_y, resNodeData->m_rotation.m_z);
        }
        for (int i = spawnersIndex + 1; i < endIndex; i++) {
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(i).ptr();
            this->createObjSpawner(resNodeData->m_rotation.m_x, resNodeData->m_translation.xy(),
                                    resNodeData->m_rotation.m_z, resNodeData->m_translation.m_z,
                                    resNodeData->m_rotation.m_y, resNodeData->m_scale.m_x, resNodeData->m_scale.m_y,
                                    resNodeData->m_scale.m_z, i != 0);
        }
    }
}

void stKrazoa::createObjLadder(int mdlIndex, Vec2f* pos, int motionPathIndex, bool restrictUpExit, bool unk2) {

    grKrazoaLadder* ladder = grKrazoaLadder::create(mdlIndex, "grKrazoaLadder");
    if (ladder != NULL) {
        addGround(ladder);
        Vec2f areaPos = Vec2f(0.0, 0.0);
        Vec2f areaRange = Vec2f(0.0, 0.0);
        grGimmickLadderData ladderData(
                mdlIndex, 0, restrictUpExit, unk2, "",
                &areaPos, &areaRange
        );
        ladder->setMotionPathData(motionPathIndex);
        ladder->startupLadder(this->m_fileData, 0, gfSceneRoot::Layer_Ground, &ladderData);
        ladder->setPos(pos->m_x, pos->m_y, 0.0);
    }
}

void stKrazoa::createObjSpawner(int mdlIndex, Vec2f* pos, float rot, int motionPathIndex, int collIndex, float minRespawnFrames, float maxRespawnFrames, float spawnAfterFrames, bool isFirstSilentDeployment) {
    grKrazoaSpawner* platform = grKrazoaSpawner::create(mdlIndex, "", "grKrazoaSpawner");
    if (platform != NULL){
        addGround(platform);
        platform->setStageData(m_stageData);
        platform->setConfig(motionPathIndex, minRespawnFrames, maxRespawnFrames, spawnAfterFrames, isFirstSilentDeployment);
        platform->startup(this->m_fileData,0,gfSceneRoot::Layer_Ground);
        platform->setPos(pos->m_x, pos->m_y, 0.0);
        platform->setRot(0.0, 0.0, rot);
        if (collIndex >= 0) {
            createCollision(m_fileData, collIndex, platform);
        }
    }
}

void stKrazoa::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID, gfArchive** commonParam, itCustomizerInterface** customizer) {
    if (itemID == Item_Stage_Shellcreeper) {
        *brres = &this->itemBrres;
        *param = &this->itemParam;
        *commonParam = &this->itemCommonParam;
    }
}

void stKrazoa::update(float frameDelta){

}

bool stKrazoa::isBamperVector() {
   return true;
}
GXColor stKrazoa::getFinalTechniqColor() {
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