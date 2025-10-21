#include <memory.h>
#include <st/st_class_info.h>
#include "st_smashketball.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>
#include <ft/ft_manager.h>

static stClassInfoImpl<Stages::Final, stSmashketball> classInfo = stClassInfoImpl<Stages::Final, stSmashketball>();

stSmashketball* stSmashketball::create() {
    stSmashketball* stage = new(Heaps::StageInstance) stSmashketball(Stages::Final);
    return stage;
}

bool stSmashketball::loading(){
    return true;
}

void stSmashketball::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);

    createObjGround(1);
    createObjGround(2);
    createObjCannon(110, 0);
    createObjCannon(110, 1);
    createObjGlass(3, 4, 0);
    createObjGlass(3, 4, 1);
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

void stSmashketball::createObjGround(int mdlIndex) {
    grFinal* ground = grFinal::create(mdlIndex, "", "grFinal");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, 0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stSmashketball::createObjCannon(int mdlIndex, int index) {

    stSmashketballData* smashketballData = (stSmashketballData*)this->m_stageData;
    
    this->cannonData[index].motionPathData = (grGimmickMotionPathData){1.0, 0, grGimmickMotionPathData::Path_Loop, 0xFF, 0};
    this->cannonData[index].areaOffsetPos = Vec2f(0.0, 0.0);
    this->cannonData[index].areaRange = Vec2f(20.0, 15.0);
    this->cannonData[index].pos = smashketballData->cannonPosData[index].pos;
    this->cannonData[index].rot = smashketballData->cannonPosData[index].rot;
    this->cannonData[index].maxRot = 62.0;
    this->cannonData[index].difficultyRotateSpeeds[0] = 0.0;
    this->cannonData[index].maxFrames = 0;
    this->cannonData[index].maxFireRot = smashketballData->cannonPosData[index].rot;
    this->cannonData[index].isAutoFire = true;
    this->cannonData[index].fullRotate = false;
    this->cannonData[index].alwaysRotate = false;
    this->cannonData[index].mdlIndex = mdlIndex;
    this->cannonData[index].breakHitstopFrame = 0x8;
    this->cannonData[index].enterCannonTriggerData = (stTriggerData){ 0, 0, 1, stTriggerData::Keep_None };
    this->cannonData[index].motionPathTriggerData = (stTriggerData){ 0, 0, 1, stTriggerData::Keep_None };
    this->cannonData[index].isValidTriggerData = (stTriggerData){ 0, 0, 1, stTriggerData::Keep_None };
    this->cannonData[index].attackData.m_damage = 0;
    this->cannonData[index].attackData.m_offsetPos.m_x = 0.0;
    this->cannonData[index].attackData.m_offsetPos.m_y = 0.0;
    this->cannonData[index].attackData.m_offsetPos.m_z = 0.0;
    this->cannonData[index].attackData.m_size = 10.0;
    this->cannonData[index].attackData.m_vector = 0x169;
    this->cannonData[index].attackData.m_reactionEffect = 0x19;
    this->cannonData[index].attackData.m_reactionFix = 0;
    this->cannonData[index].attackData.m_reactionAdd = 0x5;
    this->cannonData[index].attackData.m_attribute = soCollisionAttackData::Attribute_Normal;
    this->cannonData[index].attackData.m_isSetOffOn = false;
    this->cannonData[index].attackData.m_noSetOffThru = false;
    this->cannonData[index].attackData.m_targetSituationAir = true;
    this->cannonData[index].attackData.m_targetSituationGround = true;
    this->cannonData[index].attackData.m_serialHitFrame = 0x3c;
    this->cannonData[index].attackData.m_soundLevel = soCollisionAttackData::Sound_Level_Medium;
    this->cannonData[index].attackData.m_soundAttribute = soCollisionAttackData::Sound_Attribute_Punch;
    this->cannonData[index].attackData.m_noScale = false;
    this->cannonData[index].attackData.m_shapeType = soCollision::Shape_Sphere;
    this->cannonData[index].attackData.m_nodeIndex = 0;
    this->cannonData[index].attackData.m_power = 0x1E;
    this->cannonData[index].shootSpeed = smashketballData->shootSpeed;
    this->cannonData[index].shootTimerSpeed = smashketballData->shootTimerSpeed;
    this->cannonData[index].shootAngleOffset = 0.0;
    this->cannonData[index].shootStunTimerSpeed = smashketballData->shootStunTimerSpeed;

    grAdventureBarrelCannon* cannon = grAdventureBarrelCannon::create(this->cannonData[index].mdlIndex, BarrelCannon_Static, "grAdventureBarrelCannon");
    if (cannon != NULL)
    {
        addGround(cannon);
        cannon->setGimmickData(&this->cannonData[index]);
        cannon->startup(m_fileData, 0, 0);
    }
}

void stSmashketball::createObjGlass(int mdlIndex, int collIndex, int index) {
    stSmashketballData* smashketballData = (stSmashketballData*)this->m_stageData;
    grSmashketballGlass* glass = grSmashketballGlass::create(mdlIndex, "", "grGlass");
    if (glass != NULL)
    {
        addGround(glass);
        glass->startup(m_fileData, 0, 0);
        glass->setStageData(m_stageData);
        glass->setPos(smashketballData->glassPos[index].m_x, smashketballData->glassPos[index].m_y, 0);
        createCollision(m_fileData, collIndex, glass);
    }
}

void stSmashketball::update(float frameDelta){
    stSmashketballData* smashketballData = (stSmashketballData*)this->m_stageData;
    if (smashketballData->isDisableCollision) {
        int entryCount = g_ftManager->getEntryCount();
        for (int i = 0; i < entryCount; i++) {
            int entryId = g_ftManager->getEntryIdFromIndex(i);
            Fighter* fighter = g_ftManager->getFighter(entryId, -1);
            if (fighter->m_moduleAccesser->getStatusModule().getStatusKind() == 192) {
                fighter->m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
            };
        }
    }
}

bool stSmashketball::isBamperVector() {
   return true;
}
GXColor stSmashketball::getFinalTechniqColor() {
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