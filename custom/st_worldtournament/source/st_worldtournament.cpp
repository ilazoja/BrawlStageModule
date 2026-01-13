#include <memory.h>
#include <st/st_class_info.h>
#include "st_worldtournament.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stWorldTournament> classInfo = stClassInfoImpl<Stages::Final, stWorldTournament>();

stWorldTournament* stWorldTournament::create() {
    stWorldTournament* stage = new(Heaps::StageInstance) stWorldTournament(Stages::Final);
    return stage;
}

bool stWorldTournament::loading(){
    return true;
}

void stWorldTournament::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjConcrete(1);
    createObjConcrete(2);
    createObjConcrete(3);
    createObjConcrete(4);
    createObjConcrete(5);
    createObjConcrete(6);
    createObjConcrete(7);
    createObjConcrete(8);
    createObjConcrete(9);
    createObjGrass(10,3);
    createObjConcrete(11);
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

void stWorldTournament::createObjConcrete(int mdlIndex) {
    grWorldTournamentConcrete* concrete = grWorldTournamentConcrete::create(mdlIndex, "", "grWorldTournamentConcrete");
    if (concrete != NULL)
    {
        addGround(concrete);
        concrete->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        concrete->setStageData(m_stageData);
    }
}

void stWorldTournament::createObjGrass(int mdlIndex, int collIndex) {
    grWorldTournamentGrass* grass = grWorldTournamentGrass::create(mdlIndex, "", "grWorldTournamentGrass");
    if (grass != NULL)
    {
        addGround(grass);
        grass->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        grass->setStageData(m_stageData);
        createCollision(m_fileData, collIndex, grass);
    }
}

void stWorldTournament::update(float frameDelta){

}

bool stWorldTournament::isBamperVector() {
   return true;
}
GXColor stWorldTournament::getFinalTechniqColor() {
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