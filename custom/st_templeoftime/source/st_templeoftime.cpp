#include <memory.h>
#include <hkmath/hkMath.h>
#include <st/st_class_info.h>
#include "st_templeoftime.h"
#include <OS/OSError.h>
#include "gr_final.h"
#include "gr_templeoftime_door.h"
#include "gr_templeoftime_medallion.h"

static stClassInfoImpl<Stages::Final, stTempleOfTime> classInfo = stClassInfoImpl<Stages::Final, stTempleOfTime>();

stTempleOfTime* stTempleOfTime::create() {
    stTempleOfTime* stage = new(Heaps::StageInstance) stTempleOfTime(Stages::Final);
    return stage;
}

bool stTempleOfTime::loading(){
   return true;
}

void stTempleOfTime::createObj() {

    this->testStageParamInit(this->m_fileData, 0xA);
    this->testStageDataInit(this->m_fileData, 0x14, 1);

    this->createObjGround(1);
    this->createObjGround(2);
    this->createObjDoor(102);
    this->createObjMedallion(103);

    this->createCollision(this->m_fileData, 2, 0);
    this->initCameraParam();
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

    this->createWind2ndOnly();
    this->loadStageAttrParam(this->m_fileData, 0x1E);
    this->scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    this->registScnAnim(this->scnData, 0);
    this->initPosPokeTrainer(1, 0);
    this->createObjPokeTrainer(this->m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);
}

void stTempleOfTime::createObjGround(int mdlIndex) {
    grFinal* ground = grFinal::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stTempleOfTime::createObjDoor(int mdlIndex) {
    grTempleOfTimeDoor* door = grTempleOfTimeDoor::create(mdlIndex, "", "grTempleOfTimeDoor");
    if (door != NULL)
    {
        addGround(door);
        door->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        door->setStageData(m_stageData);
        door->initializeEntity();
        door->startEntity();
    }
}

void stTempleOfTime::createObjMedallion(int mdlIndex) {
    grTempleOfTimeMedallion* medallion = grTempleOfTimeMedallion::create(mdlIndex, "", "grTempleOfTimeMedallion", this);
    if (medallion != NULL)
    {
        addGround(medallion);
        medallion->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        medallion->setStageData(m_stageData);
        medallion->setStageParam(this->m_stageParam);
        medallion->initializeEntity();
        medallion->startEntity();
    }
}

void stTempleOfTime::update(float frameDiff){
    return;
}

bool stTempleOfTime::isBamperVector() {
   return true;
}
GXColor stTempleOfTime::getFinalTechniqColor() {
   return (GXColor){0x14000496};
}

void stTempleOfTime::changeScnAnim(int index) {
    registScnAnim(this->scnData, index);
}

template<Stages::srStageKind I, typename T>
T* stClassInfoImpl<I, T>::create(){
    T* stage = new(Heaps::StageInstance) T(I);
    return stage;
}

template<Stages::srStageKind I, typename T>
stClassInfoImpl<I, T>::~stClassInfoImpl() {
   setClassInfo(I,0);
}

template<Stages::srStageKind I, typename T>
void stClassInfoImpl<I, T>::preload() {
   return;
}

// TODO:
// Fireworks after ko? coins? hidden blocks randomly moving/item box appearing with item (maybe can spawn from taking the bad pipe)?