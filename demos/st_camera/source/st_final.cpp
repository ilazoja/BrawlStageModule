#include "st_final.h"
#include "gr_final.h"
#include <memory.h>
#include <st/st_class_info.h>
#include <cm/cm_camera_controller.h>

static stClassInfoImpl<Stages::Final, stFinal> classInfo = stClassInfoImpl<Stages::Final, stFinal>();

stFinal* stFinal::create()
{
    stFinal* stage = new (Heaps::StageInstance) stFinal();
    return stage;
}
bool stFinal::loading()
{
    return true;
}
void stFinal::update(float deltaFrame)
{
    return;
}

void stFinal::createObj()
{
    testStageParamInit(m_fileData, 0xA);
    testStageDataInit(m_fileData, 0x14, 1);
    grFinal* ground = grFinal::create(1, "", "grFinalMainBg");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setType(0);
        ground->startup(m_fileData, 0, 0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
    ground = grFinal::create(2, "", "grFinalStage");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setType(1);
        ground->startup(m_fileData, 0, 0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
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
    loadStageAttrParam(m_fileData, 0x1E);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);

    nw4r::g3d::ResFile resFile(scnData);
    nw4r::g3d::ResAnmScnData* scn0 = resFile.GetResAnmScn(0).ptr();
    CameraController* camController = CameraController::getInstance();
    camController->changeInput(CameraController::AnimationLocked);
    cmAnimationController* animController = static_cast<cmAnimationController*>(camController->m_currentController);
    animController->setScnAnmRes(scn0, 0);

}


void stFinal::startFighterEvent()
{
    return;
}
int stFinal::initializeFighterAttackRatio()
{
    return 0;
}
int stFinal::helperStarWarp()
{
    return 0;
}
bool stFinal::isSimpleBossBattleMode()
{
    return false;
}
bool stFinal::isBossBattleMode()
{
    return false;
}
bool stFinal::isCameraLocked()
{
    return true;
}
void stFinal::notifyTimmingGameStart()
{
    return;
}
float stFinal::getFrameRuleTime()
{
    return this->m_frameRuleTime;
}
void stFinal::setFrameRuleTime(float newTime)
{
    this->m_frameRuleTime = newTime;
}
bool stFinal::isNextStepBgmEqualNowStepBgmFromFlag()
{
    return false;
}
float stFinal::getBgmPlayOffsetFrame()
{
    return BGM_PLAY_OFFSET_FRAME;
}
float stFinal::getBgmVolume()
{
    return BGM_VOLUME;
}
void stFinal::setBgmChange(float unk, u32 unk1, u32 unk2)
{
    this->m_unk2 = unk1;
    this->m_unk3 = unk2;
    this->m_unk4 = unk;
}
void stFinal::getBgmChangeID(u32 unk1, float unk2)
{
    unk1 = this->m_unk3;
    unk2 = this->m_unk4;
}
bool stFinal::isBgmChange()
{
    return this->m_unk2;
}
int stFinal::getBgmOptionID()
{
    return 0;
}
int stFinal::getNowStepBgmID()
{
    return 0;
}
int stFinal::getBgmID()
{
    return 0;
}
int stFinal::getBgmIDOverload()
{
    return 0;
}
void stFinal::appearanceFighterLocal()
{
    return;
}
// TODO
// stubbed because i can't figure out where this is called
int stFinal::getScrollDir(u32 unk1)
{
    return 0;
}
int stFinal::getDefaultLightSetIndex()
{
    return 0x14;
}
Rect2D* stFinal::getAIRange()
{
    return &this->m_aiRange;
}
bool stFinal::isAdventureStage()
{
    return false;
}
int stFinal::getPokeTrainerDrawLayer()
{
    return 0;
}
float stFinal::getPokeTrainerPositionZ()
{
    return POKETRAINER_Z;
}
void stFinal::getPokeTrainerPointData(int* unk, int unk2)
{
    return;
}
int stFinal::getPokeTrainerPointNum()
{
    return 0;
}
bool stFinal::isReStartSamePoint()
{
    return true;
}
grGimmickWindData2nd* stFinal::getWind2ndOnlyData()
{
    return m_windAreaData2nd;
}
bool stFinal::isBamperVector()
{
    return true;
}
int stFinal::getFinalTechniqColor()
{
    return 0x14000496;
}

ST_CLASS_INFO;